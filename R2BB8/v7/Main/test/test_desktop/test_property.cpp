//#include "Arduino.h"
#include <unity.h>

#include "./Util/Property.h"

#ifdef UNIT_TEST

void UNITY_SETUP(){
  //delay(2000);
  UNITY_BEGIN();
}

void setup(){
  UNITY_SETUP();
    // Some Setup:
    Property<int8_t> P = Property<int8_t>("Property", 5);

    // Test Value:
    TEST_ASSERT_EQUAL_INT(P.value(), 5);
    TEST_ASSERT_EQUAL_INT(P.value(), 5); // Check twice in a row to ensure #value doesn't overwrite on a read call
    int8_t oldval = P.value(6);
    TEST_ASSERT_EQUAL_INT(oldval, 5);
    TEST_ASSERT_EQUAL_INT(P.value(), 6);

    // Test Name:
    TEST_ASSERT_EQUAL_STRING(P.name(), "Property");

  UNITY_END();
} // #setup

void loop(){

} // #loop

#endif // UNIT_TEST
