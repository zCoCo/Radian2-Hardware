'use strict';
/****
 * Head Orchestrator of All Executive Functions of the Software's Backend.
 * Primary repsonsibility is finding and pre-loading all relevant data from the
 * rootDir.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const path = require('path');
const fs = require('fs');

const exts = require('./strings.js').Extensions; // File Extensions
const files = require('./strings.js').Key_Files; // Important Files

const util = require('./util.js');

const Part = require('./part.js');
const Assembly = require('./assembly.js');
const Product = require('./product.js');

exports.rootDir = __dirname; // Root Directory Containing a Project

/* Load all Data from the Root Directory. Triggers callback when done. */
exports.begin = function(dir, callback){
  exports.rootDir = dir || exports.rootDir;

  exports.labor_rates = loadData(files.LABOR_RATES);
  exports.locations = loadData(files.LOCATIONS);

  exports.parts = loadAll('', exts.PART, Part.load);
  exports.assemblies = loadAll('', exts.ASSEMBLY, (a) => Assembly.load(this, a));
  exports.products = loadAll('', exts.PRODUCT, (p) => Product.load(this, p));

  callback();
}

/* Load Data from an Array or Object Stored as JSON in a File at the Given Address, addr,
Relative to the Root Product Directory */
function loadData(addr){
  return JSON.parse(fs.readFileSync(path.join(exports.rootDir, addr), 'utf8'));
} // #loadData

/* Loads All Files in the Given Directory, dir, Relative to the Root Product
Directory with Extension, ext, into an Object, with Each Element Keyed by
the CamelCase of its File-Name. Loading is performed by calling the given
loadFcn with the single argument of the path to the file. */
function loadAll(dir, ext, loadFcn){
  const files = util.fs.findAllDeep(ext, path.join(exports.rootDir, dir));
  const obj = {};
  for(const f in files){
    let name = util.strings.toCamelCase(path.basename(files[f], ext));
    obj[name] = loadFcn(files[f]);
  }
  return obj;
} // #loadAll
