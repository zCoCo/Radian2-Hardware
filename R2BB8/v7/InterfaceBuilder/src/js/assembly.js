'use strict';
/****
 * Classes and Associated Functions for Managing the Data Involved in an Assembly.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const fs = require('fs');

const strings = require('./strings.js').Assembly_Strings;

const FileContainer = require('./fileContainer.js');
const CostTable = require('./costTable.js');

module.exports = class Assembly extends FileContainer{
  /* Initializes an Assembly within a Maestro Context with data stored at the
   given path, addr. */
  constructor(maestro, addr, data){
    let _data = { // Default Values
      location: strings.NO_LOCATION, // TODO: Inter-assembly shipping costs

      /* Labor Entries in form: {id: "", time_mins: ""} */
      labor: [],

      /* Sub-Assembly Entries in form: {id: "", count: ""} */
      subAssemblies: [],

      /* Part Entries in form: {id: "", count: ""} */
      parts: []
    };
    $.extend(true, _data, data); // Recursively Merge Given Data onto Defaults
    super(addr, _data);

    this.maestro = maestro;
    this.totalCount = 0; // Number of this Assembly Used Across the Entire Product
  } // #constructor

  /* Cost of One Unit of the Assembly at a Given Order Volume */
  unitCost(vol){
    vol = vol || this.totalCount; // <- default value
    return this.laborSubTotal() + this.partsSubTotal(vol) + this.subassembliesSubTotal(vol);
  } // #unitCost

  /* Weight of the Assembly */
  get weight(){
    return partsWeight() + subassembliesWeight();
  } // #weight

  /* Subtotal of All Labor used in this Assembly (excluding sub-assemblies)*/
  laborSubTotal(){
    let c = 0;
    this.data.labor.forEach( (l) => {
      c += (l.time_mins / 60.0)
         * this.maestro.labor_rates[l.id].rate_per_hour
         * this.maestro.labor_rates[l.id].overheadFactor;
    });
    return c;
  } // #laborSubTotal

  /* Subtotal of All Parts used in this Assembly (excluding sub-assemblies)*/
  partsSubTotal(vol){
    vol = vol || this.totalCount; // <- default value
    let c = 0;
    this.data.parts.forEach( (p) => { c += p.count * this.maestro.parts[p.id].unitCost(vol); } );
    return c;
  } // #partsSubTotal

  /* Weight of All Parts used in this Assembly (excluding sub-assemblies)*/
  partsWeight(){
    let w = 0;
    this.data.parts.forEach( (p) => { w += p.count * this.maestro.parts[p.id].weight; } );
    return w;
  } // #partsWeight

  /* Weight of All Sub-Assemblies used in this Assembly */
  subassembliesSubTotal(vol){
    vol = vol || this.totalCount; // <- default value
    let c = 0;
    this.data.subAssemblies.forEach( (s) => { c += s.count * this.maestro.assemblies[s.id].unitCost(vol); } );
    return c;
  } // #subassembliesSubTotal

  /* Weight of All Sub-Assemblies used in this Assembly (excluding sub-assemblies)*/
  subassembliesWeight(){
    let w = 0;
    this.data.subAssemblies.forEach( (s) => { w += s.count * this.maestro.assemblies[s.id].weight; } );
    return w;
  } // #subassembliesWeight

  /* Loads the Assembly belonging to the given Product from a JSON file at the given path. */
  static load(maestro, addr){
    // Load Part Data from Accompanying JSON File:
    let data = JSON.parse(fs.readFileSync(addr, 'utf8'));
    return new Assembly(maestro, addr, data);
  } // #load
} // Class: Assembly
