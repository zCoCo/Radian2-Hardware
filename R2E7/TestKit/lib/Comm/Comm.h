/****
  * Ottia M1 Communication Protocol for Actuators receiving messages from the
  * basestation.
  * TODO: Generalize so both ends can include this codebase.
  ** Requires generalizing System (creating ABC) for use by basestation ...
  ** or just pass the Comm an OttiaM1RegisterBank.
  *
  * Author: Connor W. Colombo
  * Last Update: 9/16/2019, Colombo
 ****/
 #ifndef _M1_COMM_H
 #define _M1_COMM_H

 #include <System.h>

 /*
  * Standard format for message headers.
  */
  struct message_t{
    union messageHeader_t{
      struct headerData_t{
        uint16_t address; //      - Bus Address of Device being Targeted
        uint8_t writeData:1; //   - Writing Data to Register = 1, Reading = 0
        uint8_t registerID:7; //  - ID of Target Register
        uint8_t :0; // start new byte
        uint8_t startByte; //     - 0-indexed number of byte to start reading/writing in the register, inclusive
        uint8_t length;  //       - Number of bytes to be read/written
        uint16_t crc; //          - 2-byte Cyclic-Redundancy Checksum of header contents (and an upcoming message if writeData=1)
      } data;
      uint8_t raw[sizeof(headerData_t)];
      uint8_t raw_len; //         - Amount of currently loaded header data.
    } header;

    uint8_t data[MAX_REG_LENGTH];
    uint8_t data_len; //           - Amount of currently loaded message data.

    /* Reads in the given number of bytes from the stream into the message. If
    the header isn't full, the bytes go there first. Once full, the bytes go into
    the message data.
    Returns the number of bytes placed into the buffer.
    */
    uint8_t loadBytes(Stream& stream, const uint8_t num){
      loadBytesHelper(stream, num, header.raw, sizeof(headerData_t), &header.raw_len)
    }

  private:
    /* Reads in the given number of bytes from the stream into the given buffer
    with a max length of buff_len which currently contains only buff_fill
    elements.
    Returns the number of bytes placed into the buffer.
    */
    uint8_t loadBytesHelper(Stream& stream, const uint8_t num, uint8_t *const buff, const uint8_t buff_len, uint8_t *const buff_fill){
      static uint8_t n_hdr; // Number of bytes to load into the header
      static uint8_t bytes_read; // Number of bytes actually read into the header

      if(num > buff_len - *buff_fill){
        n_hdr = buff_len - *buff_fill;
      } else{
        n_hdr = num;
      }
      if(n_hdr > stream.available()){
        n_hdr = stream.available();
      }

      bytes_read = stream.readBytes(buff + *buff_fill, n_hdr);
      *buff_fill = *buff_fill + bytes_read;

      return bytes_read;
    }

    // TODO: !! Purge the relevant section of the buffer after a message is processed !!
    // TODO: Make sure this still works in the edge case where the first message is being sent and it's just a request for data (no message data).

  } message;

   void shiftIn

   /*
    * Computes the CRC of all Data in the Message Header (except the CRC itself).
    * If the pointer to a message is given, the CRC will also include the given
    * message in the computation.
    * Uses CRC-16 which has a 1/65536 chance of collision (failed error
    * detection) vs the 1/256 of CRC-8.
    */
   uint16_t computeCRC(uint8_t* message = nullptr, uint8_t len = 0){

     uint16_t crc = 0xFFFF;
     for(uint8_t i=0; i<(sizeof(data_t)-2); i++){
       updateCRC16(&crc, raw[i]);
     }

     for(i=0; i<len; i++){ // len>0 -> message was given, compute CRC on that too
       updateCRC16(&crc, message[i]);
     }

     return crc;
   }

   /*
    * Performs a CRC-16 update on the given (partial) CRC with the given byte, b.
    */
   void updateCRC16(uint16_t* crc, uint8_t b){
     static constexpr uint8_t divisor = 0x4B37; // divisor / generator polynomial
     *crc ^= b;
     for(uint8_t j=0; j<8; j++){ // 8-bit division
       *crc = (*crc & 1) ? (*crc >> 1) ^ divisor : (*crc >> 1);
     }
   }

   // Sets the CRC value in the Header to that of the Rest of the Data and, if
   // a pointer and length are given, the contents of the given message.
   void setCRC(uint8_t* message = nullptr, uint8_t len = 0){
     data.crc = computeCRC(message, len);
   }

   // Returns whether the CRC stored in the header matches the CRC computed
   // from the rest of the data (verifies data integrity) and, if a pointer
   // and length are given, the contents of the given message.
   bool checkCRC(uint8_t* message = nullptr, uint8_t len = 0){
     return data.crc == computeCRC(message, len);
   }
 } messageHeader;

// TODO: NEED COMMS WRAPPER FOR SEND AND READ.

  void write(){
    activate write line
    Serial.flush();
    write data
    deactivate write line
  }

  /*
   * To resychronize a device with a bus's sender, every T ms the sender will
   * transmit two message headers containing all 1's followed by a one byte
   * terminator. This guarantees that, barring data corruption, the receiving
   * device which is scanning in blocks of headers will
   *
   * In addition, in order to synchronize new devices, a blanking interval will
   * be sent by the basestation every time the device which it knew to be the
   * last in the chain no longer has TERM=1.
   *
   * This methods minimizes comms processing time for the receiving device at
   * the expense of transmitting ~15Bytes every T ms. Fortunately, T is
   * controllable by the sender and thus this cost can be reduced if neccessary
   * (ie, lots of devices on the line) at the risk of loosing some devices if
   * they fall out of sync (which should be unlikely if other protections work).
   */
  void transmitBlankingInterval(){
    static constexpr uint8_t blankingIntervalTerminator = 0xDE;
    for(uint8_t i=0; i<sizeof(messageHeader.raw); i++){
      messageHeader.raw[i] = 0xFF;
    }
  }

 /*
  * Performs a communications update.
  * Call as often as possible since even if the serial buffer is 2048B, at a
  * max transmit rate of 500kbps, the buffer will be consumed and they device
  * will likely fall out of sync in T_OSYNC = 32ms.
  // Trade on Blanking intervals or search for initiator.
  ** TODO: !! address sync issues in protocol since if buffer overflows,
  ** comms will likely remain out of sync. !!
  // TODO: Address potential out-of-sync issue for hotswapped devices (if they
  // come online in the middle of a message, they'll never be in sync).
  // ((basestation issues a sync 0xf... pulse of 2 message lengths when the
  // device the basestation knew to be last is no longer TERM?))
  // Whether currently waiting for a message header to arrive. If not, then waiting for message body.
  */
 void comm_update(){
   static bool waiting_for_header = 1;
   static uint8_t* reg_rawData; // address of raw data section of register being read/written
   static uint8_t incomingDataBuffer[MAX_REG_LENGTH]; // Static memory buffer helps fight fragmentation

   // TODO: !! SHOULD NOW BE A LOOP UNTIL ADDRESSED AND VALID MESSAGE IS FOUND OR BUFFER IS EMPTY !!
   // Read In New Message Reader and Respond if Requested:
   if(waiting_for_header && PRIMARY_USART.available() >= sizeof(messageHeader.raw)){
     PRIMARY_USART.readBytes(messageHeader.raw, sizeof(messageHeader));

     // Check for Blanking Interval
     if(messageHeader.data.address == 0xFFFF){
       Serial.readUntil()
     }

     if(messageHeader.checkHeaderCRC() && messageHeader.data.address == HAL.deviceID){
       // Fetch Raw Data Array of Indicated Register:
       reg_rawData = System.Registers.getRegisterBytesFromID(messageHeader.data.registerID);
       // Ensure All Bytes to be Accessed are in Range (must check both in case
       // of corrupted data and a start byte which is near overflow,
       // eg. start=254, len=2, start+len%256 < sizeof(data)):
       if(reg_rawData != nullptr // make sure valid register was selected
       && messageHeader.data.startByte < sizeof(reg_rawData)
       && messageHeader.data.startByte + messageHeader.data.length <= sizeof(reg_rawData)
       && messageHeader.data.length <= MAX_REG_LENGTH){ // Perform Sanity Check on Length since CRC hasn't been checked yet
         reg_rawData += messageHeader.data.startByte; // Advance pointer to start position
         if(messageHeader.data.writeData){
         // Register is being written to:
           waiting_for_header = 0; // accept this header, wait for body.
         } else if(messageHeader.checkCRC()){
         // Valid request for data was given by basestation for selected data.
           // Craft new message header (using same data space)
           // - address stays the same since it now becomes the address of the sender (target is always the basestation)
           // - register, startByte, and length all stay the same since same data is being referenced
           messageHeader.data.writeData = 1; // this message will be writing data in the sender
           // New CRC must be computed which includes the contents of the message:
           messageHeader.setCRC(reg_rawData, messageHeader.data.length);
           // Send Message Header:
           PRIMARY_USART.write(messageHeader.raw, sizeof(messageHeader.raw));
           // Send Message:
           PRIMARY_USART.write(reg_rawData, messageHeader.data.length);
         }
       }
     }
   }
   // Read In New Message Contents and Write Contents to Selected Register:
   if(!waiting_for_header && PRIMARY_USART.available() >= messageHeader.data.length){
     // Bring in the bytes:
     PRIMARY_USART.readBytes(incomingDataBuffer, messageHeader.data.length);
     // Verify integrity of message header and message before copying:
     if(messageHeader.checkCRC(incomingDataBuffer, messageHeader.data.length)){
       memcpy(reg_rawData, incomingDataBuffer, messageHeader.data.length); // copy data into selected register
       waiting_for_header = 1; // Body read. Now awaiting a new message header.
     }
   }
 }

 #endif //_M1_COMM_H
