'use strict';
/****
 * Class for Handling a Table of Costs for a Good or Service, allowing for
 * estimation using several types of interpolation.
 *  Binned: The cost of a value will be that of the entry in the costs list with
 *  the id associated with the next lower value in the volumes list. If the value
 *  given is below the lowest volume in the table, the cost associated with the
 *  lowest entry is used. Ex:
 *    volumes:   1   | 10    | 50    | 100
 *    costs:  0.9 | 0.4   | 0.2   | 0.1
 *    cost(15) = 0.4; cost(49) = 0.4; cost(50) = 0.2; cost(51) = 0.2;
 *    cost(0) = 0.9; cost(200) = 0.1;
 *
 *  Linear: The cost of a value will be linearly interpolated between the next
 *  lower and higher value. If the value is beyond the bounds of the table data,
 *  the cost associated with the nearest bound is used. Ex:
 *    volumes:   1   | 10    | 50    | 100
 *    costs:  0.9 | 0.4   | 0.2   | 0.1
 *    cost(49) = 0.205; cost(50) = 0.2; cost(75) = 0.15; cost(100) = 0.1;
 *    cost(0) = 0.9; cost(200) = 0.1;
 *
 * Authors: Connor W. Colombo (Ottia)
****/

/* Types of CostTable Interpolation */
const Type = Object.freeze({
    BINNED: Symbol("binned"),
    LINEAR: Symbol("linear"),
    NOT_A_TYPE: 0 // Not unique. Evaluates to False
});

/* Checks to see if the given string, t, matches a Type name. If so, it returns
the matching Type entry.
*/
function validateType(tstr){
  let retVal = Type.NOT_A_TYPE;
  if(tstr != Type.NOT_A_TYPE){
    for(const t in Type){
      // If a valid type (not NOT_A_TYPE) and symbol string matches given string:
      if(Type[t] && String(Type[t]).slice(7, -1) == tstr){
        retVal = Type[t];
        break;
      }
    }
  }

 return retVal;
} // #validateType

module.exports = class CostTable{
  /* Constructs CostTable from data object (constaining type, volumes array, and
  cost array). This format emerges from the object parsed from a JSON file */
  constructor(data){
    this.type = validateType(data.type);
    this.type = this.type ? this.type : Type.BINNED; // Default value
    let len = Math.min(data.volumes.length, data.costs.length);
    this.volumes = data.volumes.slice(0, len);
    this.costs = data.costs.slice(0, len);
  }

  /* Returns the Cost associated with the given volume value using the interpolation
  method specified in this.type. */
  cost(volume){
    let retVal = this.costs[0];
    switch(this.type){
      case Type.BINNED:
        let idx = -1; // Index of matching volume
        do{ idx++; } while(volume > this.volumes[idx+1] && (idx+1) < this.volumes.length);
        retVal = this.costs[idx];
      break;

      case Type.LINEAR:
        if(volume < this.volumes[0]){
          retVal = this.costs[0];
        } else if(volume > this.volumes[this.volumes.length-1]){
          retVal = this.costs[this.volumes.length-1];
        } else{
          let iu = 1; // Upper Index of Range
          while(volume > this.volumes[iu] && iu < this.volumes.length){
            iu++;
          }
          retVal = (volume - volume[iu-1]) * (cost[iu] - cost[iu-1]) / (volume[iu] - volume[iu-1]) + cost[iu-1];
        }
      break;

      case Type.NOT_A_TYPE:
      default:
        retVal = 0;
      break;
    }

    return retVal;
  } // #cost

  /* Custom JSON text */
  toJSON(){
    return {
      volumes: this.volumes,
      costs: this.costs,
      type: this.type ? String(this.type).slice(7, -1) : undefined
    }
  } // #toJSON
}
