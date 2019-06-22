'use strict';
/****
 * Classes and Associated Functions for Managing the Data Involved in a Part.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const fs = require('fs');

const strings = require('./strings.js').Part_Strings;

const FileContainer = require('./fileContainer.js');
const CostTable = require('./costTable.js');

module.exports = class Part extends FileContainer{
  /* Initializes a Part with data to be stored at the given path, addr. */
  constructor(addr, data){
    let _data = { // Default Values
      vendor: strings.NO_VENDOR,
      link: strings.NO_LINK,

      costTable: new CostTable({ // Unit Cost of Item at a Given Purchase Volume
        volumes: [0],
        costs: [0],
        type: 0
      }),

      weight: 0,

      origin: strings.NO_ORIGIN,
      sourcingProvider: strings.NO_PROVIDER,
      sourcingDestination: strings.NO_DESTINATION,
      sourcingCostTable: new CostTable({ // Bulk (net) Shipping Cost of a Given Volume of Items
        volumes: [0],
        costs: [0],
        type: 0
      })
    };
    $.extend(true, _data, data); // Recursively Merge Given Data onto Defaults
    super(addr, _data);

    this.totalPartCount = 0; // Number of this Part Used Across the Entire Product
  } // #constructor

  /* Unit Cost at a Given Order Volume */
  unitCost(vol){
    vol = vol || this.totalPartCount; // <- default value
    return this.data.costTable.cost(vol) + this.data.sourcingCostTable.cost(vol) / vol;
  } // #unitCost

  get weight(){
    return this.data.weight;
  } // #weight

  /* Loads the Part from a JSON file at the given path */
  static load(addr){
    // Load Part Data from Accompanying JSON File:
    let data = JSON.parse(fs.readFileSync(addr, 'utf8'));
    data.costTable = new CostTable(data.costTable) || new CostTable();
    data.sourcingCostTable = new CostTable(data.sourcingCostTable) || new CostTable();

    return new Part(addr, data);
  } // #load
} // Class: Part
