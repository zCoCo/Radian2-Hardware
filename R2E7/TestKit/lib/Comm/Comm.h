/****
  * Ottia M1 Communication Protocol for Actuators receiving messages from the
  * basestation.
  * TODO: Generalize so both ends can include this codebase.
  ** Requires generalizing System (creating ABC) for use by basestation ...
  ** or just pass the Comm an OttiaM1RegisterBank.
  *
  * Author: Connor W. Colombo
  * Last Update: 9/15/2019, Colombo
 ****/
 #ifndef _M1_COMM_H
 #define _M1_COMM_H

 #include <System.h>

 /*
  * Standard format for message headers.
  */
 union messageHeader_t{
   struct data_t{
     uint16_t address; //      - Bus Address of Device being Targeted
     uint8_t writeData:1; //   - Writing Data to Register = 1, Reading = 0
     uint8_t registerID:7; //  - ID of Target Register
     uint8_t :0; // start new byte
     uint8_t startByte; //     - 0-indexed number of byte to start reading/writing in the register, inclusive
     uint8_t length;  //       - Number of bytes to be read/written
     uint16_t crc; //          - 2-byte Cyclic-Redundancy Checksum of header contents (and an upcoming message if writeData=1)
   } data;
   uint8_t raw[sizeof(data_t)];

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

 /*
  * Performs a communications update.
  * Call as often as possible since max buffer size is 64bytes.
  // Trade onL Blanking intervals or search for initiator.
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

   // Read In New Message Reader and Respond if Requested:
   if(waiting_for_header && PRIMARY_USART.available() >= sizeof(messageHeader.raw)){
     PRIMARY_USART.readBytes(messageHeader.raw, sizeof(messageHeader));

     if(messageHeader.checkCRC() && messageHeader.data.address == HAL.deviceID){
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
