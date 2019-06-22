'use strict';
/****
 * Superclass for Managing the Standard Set of Data Stored in a JSON file Used
 * for Entries in the BOM such as Part and Assembly. The headline feature here
 * beyond basic data storage is the ability to store custom parametes (like
 * density and volume) and have required expressions use mathjs to evaluate an
 * expression that depends on those custom parameters (ie. weight: density*volume).
 * This includes automatic solve-order, so expression-basef parameters can be
 * entered and stored in the file in any order.
 * All parameters which are to be evaluated as expression should be stored in the
 * data.expressions object. They will be evaluated automatically at construction
 * and change (<-soon) and are accessible from the data object like all other
 * parameters.
 *
 * Authors: Connor W. Colombo (Ottia)
****/

const strings = require('./strings.js').Sys_Strings;

const fs = require('fs');
const math = require('mathjs');

module.exports = class FileContainer{
  /* Initializes a Part with the given name, and data to be stored at the given
  path, addr. */
  constructor(addr, data){
    this.addr = addr; // File Address
    this.data = { // Default Values
      name: strings.NO_NAME,
      notes: "",

      expressions: 0, // Math Expressions for Custom Data to Evaluate
    };
    $.extend(true, this.data, data); // Recursively Merge Given Data onto Defaults

    this.evaluated = {}; // List of Booleans Indicating which Expressions have
                        //  had Evaluation Attempted and Whether that Eval was
                        // Successful.
    this.evalExpressions();
  } // #constructor

  // Removes Evaluated Expressions from Part Data (to prevent name conflict with
  // the expressions themself).
  removeEvalExpressions(){
    for(const e in this.evaluated){
      delete this.data[e];
    }
  }

  /* Evaluate All Math Expressions in the Part
  ** NOTE: If an Expression has the Same Key as an Exisiting Data Value, that
  Data Value will be Overwritten **
  */
  evalExpressions(){
    if(this.data.expressions){
      this.removeEvalExpressions(); // Remove All Evaluated Expressions from Data
      this.evaluated = {}; // Mark All Expressions for Re-Evaluation

      let n_solved = 0;
      // As a crude implementation of automatic solve order, the list of
      // expressions to evaluate will be looped through until no new values can
      // be solved for. This approach may be subject to errors with co-dependent
      // variables.
      do{
        // Local Expressions Can Access Data in the Part Data or in Other Expressions:
        let scope = Object.assign({}, this.data.expressions, this.data);
        n_solved = 0;

        for(const expr in this.data.expressions){
          if(!this.evaluated[expr]){
            try{
              this.data[expr] = math.eval(this.data.expressions[expr], scope);
              this.evaluated[expr] = 1; // Mark Expression as Successfully Evaluated
              n_solved++;
            } catch(e){
              this.evaluated[expr] = 0; // Mark Expression Evaluation as Failed
              // ** Don't write to this.data here since it will override the expression
              // when the next scope is built
            }
          } //!evaluated
        } // for all expressions
      } while(n_solved > 0);

      // Indicate in the Data that Failed Expressions were a Failure:
      for(const e in this.evaluated){
        if(!this.evaluated[e]){
          this.data[e] = strings.INVALID_MATH;
        }
      }
    } // if has expressions
  } // #evalExpressions

  static load(addr){ throw new TypeError("Can't load Abstract FileContainer."
                                       + " Must be Overridden in Child.")}

  /* Saves the Given FileContainer to its associated JSON file */
  save(){
    this.removeEvalExpressions(); // Remove All Evaluated Expressions from Data
    let json = JSON.stringify(this.data, null, 2); // Save as 'pretty-print'
    fs.writeFileSync(this.addr, json, 'utf8');
  } // #save
} // class: FileContainer
