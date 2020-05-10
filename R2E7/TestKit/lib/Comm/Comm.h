/****
  * Ottia M1 Communication Protocol for Actuators receiving messages from the
  * basestation.
  * TODO: Generalize so both ends can include this codebase.
  * TODO: Disable (stepper) interrupts around register writes.
  ** Requires generalizing System (creating ABC) for use by basestation ...
  ** or just pass the Comm an OttiaM1RegisterBank.
  *
  * Author: Connor W. Colombo, Ottia
  * Last Update: 11/25/2019, Colombo
 ****/
 #ifndef _M1_COMM_H
 #define _M1_COMM_H

 #include <System.h>

 /*
  * Standard format for message contents.
  */
  struct message_t{
    static constexpr uint8_t max_data_length = MAX_REG_LENGTH;

    // Header Struct (standard format for message headers):
    struct {
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
      };
      uint8_t raw_len; //         - Amount of currently loaded header data.

      // Sets the CRC value in the Header to that of the Rest of the Header and, if
      // a pointer and length are given, the contents of the given data buffer.
      void setCRC(uint8_t* dataBuff = nullptr, uint8_t buffLen = 0){
        data.crc = computeCRC(dataBuff, buffLen);
      }
      // Returns whether the CRC stored in the header matches the CRC computed
      // from the rest of the data (verifies data integrity) and, if a pointer
      // and length are given, the contents of the given data buffer.
      bool checkCRC(uint8_t* dataBuff = nullptr, uint8_t buffLen = 0){
        return data.crc == computeCRC(dataBuff, buffLen);
      }
      /*
       * Computes the CRC of all Data in the Message Header (except the CRC itself).
       * If the pointer to a data buffer is given, the CRC will also include the
       * given data buffer in the computation.
       * Uses CRC-16 which has a 1/65536 chance of collision (failed error
       * detection) vs the 1/256 of CRC-8.
       */
      uint16_t computeCRC(uint8_t* dataBuff = nullptr, uint8_t buffLen = 0){
        uint16_t crc = 0xFFFF;
        // CRC everything in header except last two bytes (which contain the CRC):
        for(uint8_t i=0; i<(sizeof(messageHeader_t)-2); ++i){
          updateCRC16(&crc, raw[i]);
        }

        for(i=0; i<buffLen; ++i){ // buffLen>0 -> message was given, compute CRC on that too
          updateCRC16(&crc, dataBuff[i]);
        }

        return crc;
      }
      /*
       * Helper function. Performs a CRC-16 update on the given (partial) CRC
       * with the given byte, b.
       */
      void updateCRC16(uint16_t* crc, uint8_t b){
        static constexpr uint8_t divisor = 0x4B37; // divisor / generator polynomial
        *crc ^= b;
        for(uint8_t j=0; j<8; j++){ // 8-bit division
          *crc = (*crc & 1) ? (*crc >> 1) ^ divisor : (*crc >> 1);
        }
      }
    } header;

    uint8_t data[max_data_length];
    uint8_t data_len; //           - Amount of currently loaded message data.

    message_t(){
      header.data.address = 0;
      header.data.writeData = 0;
      header.data.registerID = 0;
      header.data.startByte = 0;
      header.data.length = 0;

      // Make Sure initial CRC is wrong:
      header.setCRC();
      header.data.crc += 1;

      // Currently no data has been loaded:
      header.raw_len = 0;
      data_len = 0;
    }

    /* Shifts the given number of bytes out of the message, starting with the
    bytes in the header.
    Example: shiftOutBytes(2):
    HEADER   DATA             HEADER   DATA
    -------  -----------  ->  -------  -------------
    A B C D  E F G [] []      C D E F  G [F] [G] [] []
                (header.raw_len-1) -^  ^- (data_len-1)
    [] = empty data, [X] = data considered empty but actually still contains X.

    Note: The general principle of operation here is that excess data at the end
    of the buffers is "removed" by setting raw_len or data_len to a position
    before it (ie. most efficient way is to just leave the old data there until
    it needs to be overwritten and ignore it in the meantime).

    Assumptions:
    External processes which load the data should load in such a way that, if the
    header is not full, there is no data in the data buffer (that is, fill up
    header first).
    */
    void shiftOutBytes(const uint8_t num){
      static uint8_t i;
      static uint8_t n_copy; // amount of data to copy from data buffer
      static uint8_t offset; // where to start copying from in the data buffer

      // SHIFT HEADER OUT:
      // Update raw_len:
      if(num < header.raw_len){
        header.raw_len = header.raw_len-num;
      } else {
        header.raw_len = 0;
      }
      for(i=0; i<header.raw_len; ++i){
        header.raw[i] = header.raw[i+num];
      }

      // SHIFT DATA INTO HEADER:
      // This is safe even if data_len=0 and num < sizeof(header.raw):
      if(num > sizeof(header.raw)){
        offset = sizeof(header.raw) - num;
      } else{
        offset = 0;
      }
      n_copy = sizeof(header.raw) - header.raw_len; // Number of bytes to copy
      if(n_copy > data_len - offset){ // Make sure there's enough data to fill header
        n_copy = data_len - offset;
      }
      for(i=0; i<n_copy; ++i){
        header.raw[header.raw_len+i] = data[i+offset];
      }
      header.raw_len += n_copy; // Update amount of data held in header buffer

      // SHIFT REMAINING DATA IN THE DATA BUFFER:
      data_len -= n_copy + offset; // Update amount of data that should be held in data buffer
      for(i=0; i<data_len; ++i){
        data[i] = data[i+n_copy+offset];
      }
    }

    /* Reads in the given number of bytes from the stream into the message. If
    the header isn't full, the bytes go there first. Once full, the bytes go into
    the message data.
    If num isn't given, it will try to load as much data as possible.
    Returns the number of bytes not placed into a buffer (either b/c buffer is full or stream is empty).
    */
    uint8_t loadBytes(Stream& stream, uint8_t num = 0xFF){
      // Push bytes to the header first:
      num = num - loadBytesHelper(stream, num, header.raw, sizeof(headerData_t), &header.raw_len);
      // If any bytes left to load, place them in the message data:
      if(num > 0){
        num = num - loadBytesHelper(stream, num, data, max_data_length, &data_len);
      }

      return num;
    }

  private:
    /* Helper function for #loadBytes.
    Reads in the given number of bytes from the stream into the given buffer
    with a max length of buff_len which currently contains only buff_fill
    elements.
    Returns the number of bytes placed into the buffer.
    */
    uint8_t loadBytesHelper(Stream& stream, const uint8_t num, uint8_t *const buff, const uint8_t buff_len, uint8_t *const buff_fill){
      static uint8_t n_hdr; // Number of bytes to load into the header
      static uint8_t bytes_read; // Number of bytes actually read into the header

      // Fill up to the end of the buffer, unless there aren't enough bytes
      // requested to do that:
      if(num > buff_len - *buff_fill){
        n_hdr = buff_len - *buff_fill;
      } else{
        n_hdr = num;
      }
      // If there aren't enough bytes in the stream to load the selected amount,
      // load all bytes in the stream.
      if(n_hdr > stream.available()){
        n_hdr = stream.available();
      }

      if(n_hdr){
        bytes_read = stream.readBytes(buff + *buff_fill, n_hdr);
        *buff_fill = *buff_fill + bytes_read;
      } else{
        bytes_read = 0;
      }

      return bytes_read;
    }

    // TODO: !! Purge the relevant section of the buffer after a message is processed !!
    // TODO: Make sure this still works in the edge case where the first message is being sent and it's just a request for data (no message data).

  };

// TODO: NEED COMMS WRAPPER FOR SEND AND READ.

  /*
   * Wrapper for Common Protocol Features used by both the Basestation and
   * Devices.
   */
  class CommunicationLine{
    static constexpr uint8_t blankingIntervalTerminator = 0xDE;
    message_t message; // Contains actively handled message (persistent object helps fight fragmentation)
    // Stream over which communication occurs (note, this does not necessarily
    // line up with an RS-485 line, eg. on an M1C1v2 there is the primary
    // hardware USART stream has its read line on one RS-485 bus and the write
    // line on another).
    Stream stream;
    uint8_t pin_re; // Pin which activates the read line (active-low assumed).
    uint8_t pin_de; // Pin which activates the transmit line.
    long baud; // Assumed baud rate for the Serial stream.

    CommunicationLine(Stream& s, uint8_t re, uint8_t de, long b){
      stream = s;
      pin_re = re;
      pin_de = de;
      baud = b;
    }

  public:
    // Sets the read line state (1 = activated, 0 = deactivated)
    void setRead(uint8_t state){
      digitalWrite(pin_re, !state); // _RE is assumed to be active-low
    }
    // Sets the transmit line state (1 = activated, 0 = deactivated)
    void setWrite(uint8_t state){
      digitalWrite(pin_de, state);
    }
    // Initializes this communication line.
    void begin(){
      pinMode(re, OUTPUT);
      setRead(true);
      setWrite(false); // Deactivate transmit (only activate as needed).
      pinMode(de, OUTPUT);

      stream.begin(baud);
    }

    // Writes the given byte to the line
    void write(const uint8_t b){
      setWrite(true);
      stream.flush(); // wait for all currently staged outgoing data to finish sending
      stream.write(b);
      setWrite(false);
    }
    // Writes the given buffer with the given length to the line
    void write(uint8_t* buff, const uint8_t len){
      setWrite(true);
      stream.flush(); // wait for all currently staged outgoing data to finish sending
      stream.write(buff, len);
      setWrite(false);
    }

    /*
     * Performs a communications update.
     * Call as often as possible since even if the serial buffer is 2048B, at a
     * max transmit rate of 500kbps, the buffer will be consumed and the device
     * will likely fall out of sync in T_OSYNC = 32ms.
     */
    void update(){
      static uint8_t i; // used in loops as needed (persistent variable helps fight fragmentation)
      static uint8_t* reg_rawData; // address of raw data section of register being read/written
      static uint8_t crc_pass; // whether the CRC has been passed.
      static uint8_t is_blanking = 0; // 0xFF if currently inside a blanking interval

      if(is_blanking == 0xFF){
        if(stream.find(blankingIntervalTerminator)){
          is_blanking = 0;
        }
      }

      // If (no longer) inside blanking interval:
      if(is_blanking != 0xFF){
        // Load as much data as possible into buffer:
        // (do it here at the beginning instead of at end of last pass to allow
        // stream to fill up while other processes are running).
        message.loadBytes(stream);
      }

      // Resync process (if synced and uncorrupted, this just checks the CRC which passes):
      crc_pass = false;
      // While CRC hasn't passed yet (complete message hasn't been collected and synchronized yet):
      while(!crc_pass && is_blanking != 0xFF){
        // Detect blanking interval:
        if(message.header.raw_len == sizeof(message.header.raw)
           && message.data_len == sizeof(message.data)
           && message.header.data.address == 0xFFFF
           && message.header.data.crc == 0xFFFF
        ){ // Probable blanking interval detected:
          // Scan all bytes to confirm:
          is_blanking = 0xFF;
          for(i=2; i<sizeof(message.header.raw); ++i){
            is_blanking &= message.header.raw[i];
          }
          for(i=0; i<sizeof(message.data); ++i){
            is_blanking &= message.data[i];
          }
        } else if(
          // Header is full:
          message.header.raw_len == sizeof(message.header.raw)
          // And either this message is just a read request:
          && (!message.header.data.writeData
          // or this message is a write request
          || message.header.data.writeData && (
             // And either all bytes which will be written have been sent and stored in data buffer
             message.data_len >= message.header.data.length
             // or, the number of bytes to be read out of data buffer is bigger than the buffer itself (so, obviously header is garbage):
             || message.header.data.length > sizeof(message.data)
             )
          )
        ){
          if(message.header.data.length > sizeof(message.data)){ // If the expected data length is obviously garbage,
            message.shiftOutBytes(1); // Out of sync with bus. Shift out first byte in attempt to resync with bus.
          } else{
            // Check CRC of all relevant data:
            if(message.header.data.writeData){
              crc_pass = message.header.checkCRC(message.data, message.header.data.length); // CRC header and data buffer
            } else{
              crc_pass = message.header.checkCRC(); // CRC is only of header
            }
            // If new CRC failed, shift out first byte in attempt to resync with bus.
            if(!crc_pass){
              message.shiftOutBytes(1);
            }
          }
        } else{
          // Not enough data to determine sync, try loading more data (as much as possible):
          // (if more data can't be loaded, there's nothing more that can be done
          // to parse message by loading data - more data is needed but there isn't
          // more data to load. Best approach now is to exit and attempt again on
          // next function call when more data is available).
          uint8_t chunkSize = min(0xFF, stream.available()); // Amount of data to attempt to load
          if((chunkSize - message.loadBytes(stream, chunkSize)) == 0){
             // No data was loaded (either due to empty stream or full buffer):
            break;
          }
        }
      }

      if(crc_pass){

      }
    }

  private:

  }; // Class CommunicationLine

  #ifdef M1_IS_DEVICE
    /*
     * Wrapper for the Protocol used by the a Device (eg. actuator) to Communicate
     * with the Basestation. (note: per actuator, there are two communication lines.)
     */
    class DeviceCommunicationLine(){
    public:
      uint16_t address; // Device Address
      DeviceCommunicationLine(uint16_t addr, Stream& s, uint8_t re, uint8_t de, long b)
      : CommunicationLine(Stream& s, uint8_t re, uint8_t de, long b),
      address{addr} {};

      void update(){
        if(CommunicationLine::update()){ // If message aligned and CRC passed:
          // CRC Passed -> Message is uncorrupted and in sync. Handle Message as Normal:
          if(message.header.data.address == address){ // If this device is being addressed:
            // Fetch Raw Data Array of Indicated Register:
            reg_rawData = System.Registers.getRegisterBytesFromID(message.header.data.registerID);
            // Ensure All Bytes to be Accessed are in Range (must check both in case
            // of corrupted data which passed CRC by chance and a start byte which is
            // near overflow, eg. start=254, len=2, start+len%256 < sizeof(data)):
            if(reg_rawData != nullptr // make sure valid register was selected
            && message.header.data.startByte < sizeof(reg_rawData)
            && message.header.data.startByte + message.header.data.length <= sizeof(reg_rawData)
            && message.header.data.length <= MAX_REG_LENGTH){
              reg_rawData += message.header.data.startByte; // Advance pointer to start position
              if(message.header.data.writeData){
                memcpy(reg_rawData, message.data, message.header.data.length); // copy data into selected register
              } else{
                // Valid request for data was given by basestation for selected data.
                // Craft new message header (using same data space)
                // - address stays the same since it now becomes the address of the sender (target is always the basestation)
                // - register, startByte, and length all stay the same since same data is being referenced
                message.header.data.writeData = 1; // this message will be writing data in the sender
                // New CRC must be computed which includes the contents of the message:
                message.header.setCRC(reg_rawData, message.header.data.length);
                // Send Message Header:
                write(message.header.raw, sizeof(message.header.raw));
                // Send Message:
                write(reg_rawData, message.header.data.length);
              }
            }

            // Shift out all data used by the message.
            // If data is clean, this will automatically align the next message:
            message.shiftOutBytes(sizeof(message.header.raw) + message.header.data.writeData ? message.header.data.length : 0);
          }
        }
      }
    }; // Class DeviceCommunicationLine
  #endif // M1_IS_DEVICE

  #ifdef M1_IS_BASESTATION
    /*
     * Wrapper for the Protocol used by the Basestation to Communicate with
     * Devices (eg. actuators).
     * (note: there are 4 primary communication lines on the basestation)
     */
    class BasestationCommunicationLine(){
    public:
      BasestationCommunicationLine(Stream& s, uint8_t re, uint8_t de, long b)
      : CommunicationLine(Stream& s, uint8_t re, uint8_t de, long b){};

      // Maximum Number of Devices on the Communication Line:
      static constexpr uint8_t MAX_DEVICES_PER_LINE = 25;
      // Register Banks:
      OttiaM1RegisterBank RegisterBanks[MAX_DEVICES_PER_LINE];
      // Address of Each (nth) Device on the Communication Line:
      uint16_t AddressMap[MAX_DEVICES_PER_LINE];
      // Persistent container for any outgoing message on this line:
      message_t outgoingMessage;
      // Time when last message was sent on this communication line:
      unsigned long last_send = 0;
      // Amount of Time Since Last Send on the CommunicationLine after which a
      // Blanking Interval will be Sent [ms]:
      unsigned long blankingIntervalDelay = 500;

      // Finds the index of the devices with the given address in this bus's
      // actuator map.
      // Returns i=MAX_DEVICES_PER_LINE if address is not found.
      uint8_t addressToIndex(uint16_t addr){
        static uint8_t i;

        i=0;
        while(i<(sizeof(AddressMap)/sizeof(AddressMap[0])){
          if(AddressMap[i] == addr){
            break;
          } else{
            ++i;
          }
        }

        return i;
      }

      /*
       * To resychronize a device with a bus's sender, every T ms without any other
       * data having been sent, the sender will transmit two message headers
       * containing all 1's followed by a one byte terminator. This guarantees
       * that, barring data corruption, the receiving device which is scanning in
       * blocks of headers will resynchronize with the bus after the blanking
       * interval.
       *
       * In addition, in order to synchronize new devices, a blanking interval will
       * be sent by the basestation every time the device which it knew to be the
       * last in the chain no longer has TERM=1.
       *
       * Blanking interval is only required to handle the edge case where:
       *   - A new device comes online and catches only the end of a previous message.
       *   - Then only one more message is sent, addressed to this new device.
       *   -- New device won't shift out bytes from old message and recognize this new
       *      message until more data is sent if:
             --- The new message has a data payload size which prevents the data buffer from being filled.
             --- The "header" which contains the tail end of the old message and part of the new header
                 happens to be read in such a way (ie the bytes line up in such a way) that
                 `message.header.data.length` is a number < the max data buffer size, sizeof(message.data),
                 but greater than the amount of the data buffer filled by the new message.
       *    - Solutions:
             -- Basestation sends blanking interval when the device the it knew to be
                last in the chain is no longer TERM.
                Also, could be benefitial to send at some regular interval in which
                no other data has been sent (ie. time_since_last_message > 500ms) to
                prevent a similar situation where the device became out of sync due
                to noise.
             -- Two CRCs (one of message header, one of data) <- limits max data speed.
       *  No other edge cases are known where a blanking interval would be required.
       *
       * This methods minimizes comms processing time for the receiving device at
       * the expense of transmitting ~15Bytes every T ms. Fortunately, T is
       * controllable by the sender and thus this cost can be reduced if necessary
       * (ie, lots of devices on the line) at the risk of losing some devices if
       * they fall out of sync (which should be unlikely if other protections work).
       */
      void transmitBlankingInterval(){
        static uint8_t i,n;

        for(n=0; n<2; ++n){
          // Write header:
          for(i=0; i<sizeof(outgoingMessage.header.raw); ++i){
            outgoingMessage.header.raw[i] = 0xFF;
          }
          write(outgoingMessage.header.raw, sizeof(outgoingMessage.header.raw));
          // Write data:
          for(i=0; i<sizeof(outgoingMessage.data); ++i){
            outgoingMessage.data[i] = 0xFF;
          }
          write(outgoingMessage.header.raw, sizeof(outgoingMessage.header.raw));
        }
        write(blankingIntervalTerminator);
      }

      /*
       * Send data to the device with the given address.
       */
      void send(uint16_t addr){

        last_send = millis();
      }

      /*
       * Request Data from the device with the given address.
       */
      void requestData(uint16_t addr){

        last_send = millis();
      }

      void update(){
        static uint8_t idx;

        if(millis() - last_send > blankingIntervalDelay){
          transmitBlankingInterval();
        }

        if(CommunicationLine::update()){ // If message aligned and CRC passed:
          // CRC Passed -> Message is uncorrupted and in sync.
          // Find RegisterBank associated with the device sending the message:
          idx = addressToIndex(message.header.data.address);
          if(idx < MAX_DEVICES_PER_LINE){ // Index was found
            // Fetch Raw Data Array of Indicated Register:
            reg_rawData = RegisterBanks[idx].getRegisterBytesFromID(message.header.data.registerID);
            // Ensure All Bytes to be Accessed are in Range (must check both in case
            // of corrupted data which passed CRC by chance and a start byte which is
            // near overflow, eg. start=254, len=2, start+len%256 < sizeof(data)):
            if(reg_rawData != nullptr // make sure valid register was selected
            && message.header.data.startByte < sizeof(reg_rawData)
            && message.header.data.startByte + message.header.data.length <= sizeof(reg_rawData)
            && message.header.data.length <= MAX_REG_LENGTH){
              reg_rawData += message.header.data.startByte; // Advance pointer to start position
              if(message.header.data.writeData){
                memcpy(reg_rawData, message.data, message.header.data.length); // copy data into selected register
              }
            }
          }

          // Shift out all data used by the message.
          // If data is clean, this will automatically align the next message:
          message.shiftOutBytes(sizeof(message.header.raw) + message.header.data.writeData ? message.header.data.length : 0);
        }
      }

    private:
      // Time when last message was sent on this communication line:
      unsigned long last_send = 0;
    }; // Class BasestationCommunicationLine
  #endif // M1_IS_BASESTATION

 #endif //_M1_COMM_H
