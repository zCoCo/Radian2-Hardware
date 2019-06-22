'use strict';
/****
 * Collection of strings used across the program, stored in one location for
 * easy updating, comparison, and localization.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const exports_EN = {};

exports_EN.Extensions = {
  PART: ".part",
  ASSEMBLY: ".assem",
  PRODUCT: ".product"
}

exports_EN.Key_Files = {
  LABOR_RATES: "labor.rates",
  LOCATIONS: "locations.json"
}

exports_EN.GUI_Strings = {
  NAME: "Ottia Ace", // Supply Chain?
  DESC: "Adaptive Cost Estimator",

  PRODUCT_SUMMARY: "Product Summary",
  DATA_ANALYSIS: "Data Analysis",
  ASSEMBLIES: "Assemblies",
  PARTS: "Parts",
  LOCATIONS: "Locations",
  SHIPMENTS: "Shipments"
}

exports_EN.Sys_Strings = {
  NO_NAME: "No Name",
  INVALID_MATH: "[EXP ERR]"
}

exports_EN.Assembly_Strings = {
  NO_LOCATION: "No Location Given"
}

exports_EN.Part_Strings = {
  NO_VENDOR: "No Vendor",
  NO_LINK: "[NEED LINK]",
  NO_ORIGIN: "No Origin",
  NO_PROVIDER: "No Provider",
  NO_DESTINATION: "No Destination"
}

exports_EN.Errors = {
}

module.exports = exports_EN;
