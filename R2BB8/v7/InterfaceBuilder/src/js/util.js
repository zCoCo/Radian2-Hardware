'use strict';
/****
 * Basic Utilities used Throughout Aba.
 *
 * Author: Connor W. Colombo (Ottia)
****/

const fs = require('fs');
const path = require('path');

////
// String Functions:
////
exports.strings = {
  toCamelCase: function(str){
    return str.replace(
      /(?:^\w|[A-Z]|\b\w)/g,
      (l, i) => i ? l.toUpperCase() : l.toLowerCase()
    ).replace(/\s+|[\W_]+/g, '');
  } // #toCamelCase
}

////
// File Management Functions:
////
exports.fs = {};

/* Finds Every File with Extension, ext, in the Given Directory, dir. Calls
callback on each find. */
function findAllHelper(ext, dir, callback){
  if(fs.existsSync(dir)){
    let files = fs.readdirSync(dir);
    for(let i=0; i<files.length; i++){
      let addr = path.join(dir, files[i]);
      if(!fs.lstatSync(addr).isDirectory() && addr.indexOf(ext) >= 0){
        callback(addr);
      }
    }
  }
} // #findAllHelper
/* Returns a List of Every File with Extension, ext, in the Given Directory, dir. */
exports.fs.findAll = function(ext, dir){
  let list = [];
  findAllHelper(ext, dir, (addr) => list.push(addr));
  return list;
} // #findAll

/* Finds Every File with Extension, ext, in the Given Directory, dir, and its
Sub-directories. Calls callback on each find. */
function findAllHelperDeep(ext, dir, callback){
  if(fs.existsSync(dir)){
    let files = fs.readdirSync(dir);
    for(let i=0; i<files.length; i++){
      let addr = path.join(dir, files[i]);
      if(fs.lstatSync(addr).isDirectory()){
        findAllHelperDeep(ext, addr, callback);
      } else if(addr.indexOf(ext) >= 0){
        callback(addr);
      }
    }
  }
} // #findAllHelperDeep
/* Returns a List of Every File with Extension, ext, in the Given Directory, dir,
 and its Sub-directories. */
exports.fs.findAllDeep = function(ext, dir){
  let list = [];
  findAllHelperDeep(ext, dir, (addr) => list.push(addr));
  return list;
} // #findAllDeep
