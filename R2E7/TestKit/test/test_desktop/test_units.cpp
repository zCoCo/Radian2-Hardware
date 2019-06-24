/*
 * Unit tests to verify functionality of the units system using strong types.
 *
 * Author: Connor W. Colombo, Ottia
 * Last Update: 6/23/2019
 */

#ifdef UNIT_TEST

#include <unity.h>

//#include <Units.h>

void test_units_addition(void){
  TEST_ASSERT_EQUAL_INT(1,1);
}

int main( int argc, char **argv) {
  UNITY_BEGIN();

  //RUN_TEST(test_units_addition);
  TEST_ASSERT_EQUAL_INT(1,1);

  UNITY_END();
}

#endif // UNIT_TEST
