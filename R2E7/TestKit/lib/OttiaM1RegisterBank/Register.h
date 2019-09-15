/****
 * Basic Representation of a Register
 *
 * Author: Connor W. Colombo
 * Last Update: 9/13/2019, Colombo
****/

#ifndef _REGISTER_H
#define _REGISTER_H

template <typename T>
struct Register{
  union{ // anonymous union
    unsigned char raw[sizeof(T)]; //  - Raw data as bytes
    T data; //                        - Core representation of data (often a struct of fields)
  };
  unsigned char id;
};

#endif // _REGISTER_H
