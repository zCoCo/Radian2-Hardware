'use strict';
/****
 * Classes and Associated Functions for Loading and Managing the Data Involved
 * in a Product.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const fs = require('fs');
const path = require('path');

const strings = require('./strings.js').Sys_Strings;

const FileContainer = require('./fileContainer.js');

module.exports = class Product extends FileContainer{
  /* Initializes a Product within a Maestro Context with data stored at the
   given path, addr. */
  constructor(maestro, addr, data){
    let _data = { // Default Values
      minManufacturingVolume: 1,

      packagedVolume: 0, // Volume (in cu.in of packaged product)

      /* Labor Entries in form: {locId: "", stockFrac: ""} */
      distributorLocations: [],

      /* Labor Entries in form: {id: "", time_mins: ""} */
      labor: [],

      // List of Ids for Assemblies Contained in this Product
      /* Assembly Entries in form: {id: "", count: ""} */
      assemblies: [],

      // List of Ids for Shipments Necessary for Making this Product
      /* Shipment Entries in form: {type: "", from: "", to: "", via: "", contents: ""} */
      shipments: []
    };
    $.extend(true, _data, data); // Recursively Merge Given Data onto Defaults
    super(addr, _data);

    this.maestro = maestro;
    this.rootDir = path.dirname(addr);
  } // #constructor

  /* Cost of Goods Sold for One Unit of the Product at a Given Order Volume */
  COGS(vol){
    vol = vol || this.totalCount; // <- default value
    return this.laborSubTotal() + this.assembliesSubTotal(vol);
  } // #unitCost

  /* Weight of the Product */
  get weight(){
    let w = 0;
    this.data.assemblies.forEach( (s) => { w += s.count * this.maestro.assemblies[s.id].weight; } );
    return w;
  } // #weight

  /* Subtotal of All Labor used in the Final Combining (likely packaging) of
  Sub-Assemblies*/
  laborSubTotal(){
    let c = 0;
    this.data.labor.forEach( (l) => {
      c += (l.time_mins / 60.0)
         * this.maestro.labor_rates[l.id].rate_per_hour
         * this.maestro.labor_rates[l.id].overheadFactor;
    });
    return c;
  } // #laborSubTotal

  /* Weight of All Sub-Assemblies used in this Assembly */
  assembliesSubTotal(vol){
    vol = vol || this.totalCount; // <- default value
    let c = 0;
    this.data.assemblies.forEach( (s) => { c += s.count * this.maestro.assemblies[s.id].unitCost(vol); } );
    return c;
  } // #assembliesSubTotal

  /* Loads the Part from a JSON file at the given path */
  static load(maestro, addr){
    // Load Part Data from Accompanying JSON File:
    let data = JSON.parse(fs.readFileSync(addr, 'utf8'));
    return new Product(maestro, addr, data);
  } // #load
} // Class: Product
