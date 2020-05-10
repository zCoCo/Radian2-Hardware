/*
 * Unit tests to verify core and supporting functionality for the communications
 * architecture and infrastructure. Can't test actual comms without external
 * device on RS485 bus.
 *
 * Author: Connor W. Colombo, Ottia
 * Last Update:   11/23/2019, Colombo
 */

#ifdef UNIT_TEST_COMMS

#include <Arduino.h>
#include <unity.h>
#include <Streaming.h>

#include <Comm.h>

//Perform any setup neccessary for testing that can't be guaranteed outside it.
void init_testing_env(void){

}

/*
 * Passes if shiftOutBytes works properly.
 */
void test_shiftOutBytes(void){
  Serial.println("Testing Helper Function: shiftOutBytes . . .");

  // Check basic testing assumptions:
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.header.raw[1],
    "This test assumes that maxlen(message header) > maxlen(message data). Why isn't it?"
  ); // This should be the case for message transmission efficiency to be at all reasonable.

  uint8_t i;
  message_t message;
  // Create a randomized reference message to test against:
  message_t ref_msg;
  for(i=0; i<sizeof(ref_msg.header.raw); ++i){
    ref_msg.header.raw[i] = random(0, 256);
  }
  ref_msg.header.raw_len = sizeof(ref_msg.header.raw);
  for(i=0; i<message_t.max_data_length; ++i){
    ref_msg.data[i] = random(0, 256);
  }
  ref_msg.data_len = max_data_length;

  // Test basic shift by 1:
  Serial.println("   > shiftOutBytes(1) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(1);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.header.raw[1],
    "      >> Failed to shift header pos [1] into [0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[sizeof(message.header.raw)-1], ref_msg.data[0],
    "      >> Failed to shift data[0] into the end of the header."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data[0], ref_msg.data[1],
    "      >> Failed to shift data pos [1] into [0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, ref_msg.header.raw_len,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, ref_msg.data_len-1,
    "      >> Failed to update data_len correctly."
  );

  // Test shift by header_len-1:
  Serial.println("   > shiftOutBytes(header length - 1) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(sizeof(ref_msg.header.raw)-1);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.header.raw[sizeof(ref_msg.header.raw)-1],
    "      >> Failed to shift header pos [end] into [0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[1], ref_msg.data[0],
    "      >> Failed to shift data[0] into header [1]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data[0], ref_msg.data[sizeof(ref_msg.header.raw)-1],
    "      >> Failed to shift data properly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, ref_msg.header.raw_len,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, ref_msg.data_len-sizeof(ref_msg.header.raw)+1,
    "      >> Failed to update data_len correctly."
  );

  // Test shift by header_len (ie, shift out the entire header).
  Serial.println("   > shiftOutBytes(header length) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(sizeof(ref_msg.header.raw));
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.data[0],
    "      >> Failed to shift data[0] into header[0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[sizeof(message.header.raw)-1], ref_msg.data[sizeof(ref_msg.header.raw)-1],
    "      >> Failed to shift the rest data into the header."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data[0], ref_msg.data[sizeof(ref_msg.header.raw)],
    "      >> Failed to shift the data remaining in data properly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, ref_msg.header.raw_len,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, ref_msg.data_len-sizeof(ref_msg.header.raw),
    "      >> Failed to update data_len correctly."
  );

  // Shift out everything but one 1 byte:
  Serial.println("   > shiftOutBytes(everything but 1 byte) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(sizeof(ref_msg.header.raw) + sizeof(ref_msg.data) - 1);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.data[sizeof(ref_msg.data)-1],
    "      >> Failed to shift data[end] into header[0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, 1,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, 0,
    "      >> Failed to update data_len correctly."
  );

  // Shift out everything but one 1 byte:
  Serial.println("   > shiftOutBytes(everything) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(sizeof(ref_msg.header.raw) + sizeof(ref_msg.data));
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, 0,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, 0,
    "      >> Failed to update data_len correctly."
  );

  // Shift out everything but one 1 byte:
  Serial.println("   > shiftOutBytes(more than everything) . . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.shiftOutBytes(sizeof(ref_msg.header.raw) + sizeof(ref_msg.data) + 5);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, 0,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, 0,
    "      >> Failed to update data_len correctly."
  );

  // Test shift by header_len-1 if there's nothing in the databuffer:
  Serial.println("   > shiftOutBytes(header length - 1) with empty data buffer. . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.data_len = 0;
  message.shiftOutBytes(sizeof(ref_msg.header.raw)-1);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw[0], ref_msg.header.raw[sizeof(ref_msg.header.raw)-1],
    "      >> Failed to shift header pos [end] into [0]."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, 1,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, 0,
    "      >> Failed to keep data_len at 0."
  );

  // Test shift by header_len-1 if there's nothing in the databuffer:
  Serial.println("   > shiftOutBytes(2) when header only contains 1 byte and data buffer is empty. . .");
  memcpy(&message, &ref_msg, sizeof(ref_msg)); // Copy ref into main message
  message.header.raw_len = 1;
  message.data_len = 0;
  message.shiftOutBytes(sizeof(ref_msg.header.raw)-1);
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.header.raw_len, 0,
    "      >> Failed to update header.raw_len correctly."
  );
  TEST_ASSERT_EQUAL_INT_MESSAGE(message.data_len, 0,
    "      >> Failed to keep data_len at 0."
  );
}

void setup() {
  delay(2100); // wait for Serial connection to be established the bad way
  UNITY_BEGIN();

  Serial.println("Starting Tests . . .");
  // Setup Testing Environment:
  init_testing_env();

  Serial.println("Testing Helper Functions . . .");
  RUN_TEST(test_shiftOutBytes);

  UNITY_END();
}

void loop(){
  // Do nothing.
}

#endif // UNIT_TEST_COMMS
