/* Generally Useful Functionality for Storing and Manipulating Strings */

#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include <WString.h>

/*
 Stores the Given String in PROGMEM Flash.
 This macro exists in case MCUs are changed and the flash storage technique
 is different.
*/
#define SS(s) F(s)

#endif // _STRING_UTILS_H
