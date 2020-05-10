/* Simplified Hefty Information Tranmission protocol:
 *  Sends a '#' character (byte) to initiate message, sends a 1 byte receiver address, sends a 1 byte command identifier, sends a 1 byte message length (N), sends a N byte message (ex. H and L uint16).
 *  ^ obviously bad and not robust, especially for a hot-swappable multidrop bus like this where it cause major synchronization issues. We're just going to pretend that's not an issue here and use this for basic functionality testing and early prototyping.
 *
 * This class should be able to be included and used by all devices on the line to ensure there aren't interpreter mismatches.
 */

#ifndef SHIPT_COMMS_H
#define SHIPT_COMMS_H

  // Helper Union to destructure the given type (ex. convert a float to a byte array).
  template <typename T>
  union ByteUnion{
    uint8_t data[sizeof(T)];
    T val;
  };

  // LED Colors (used later):
  enum COLOR : uint32_t {
    color_off = 0x000000,
    color_white = 0xFFFFFF,
    color_error = 0xFF0000, //red
    color_idle = 0x0000FF, // blue
    color_active = 0x00FF00 // green
  };

  typedef uint8_t address_t;

  // Template for Commands:
  struct Command {
    uint8_t id; // command identifier
    uint8_t len; // length of associated message

    // Define Predefined commands:
    // Motion:
    static const Command move_to;
    static const Command set_speed;

    // Signalling:
    static const Command led_color;
    static const Command led_intensity;

    // Debugging + Low-Level Control:
    static const Command debug;
    static const Command ping;
    static const Command prog;
  };
  // Initialize Predefined commands:
  // Motion:
  const Command Command::move_to = { .id = 0x00, .len = sizeof(float) }; // [deg]
  const Command Command::set_speed = { .id = 0x01, .len = sizeof(uint16_t) }; // [deg/s]

  // Signalling:
  const Command Command::led_color = { .id = 0x10, .len = sizeof(COLOR) };
  const Command Command::led_intensity = { .id = 0x11, .len = sizeof(uint8_t) }; // 0 to 255

  // Debugging + Low-Level Control:
  const Command Command::debug = { .id = 0xF0, .len = 32 }; // pass along a debug char[] of length 32
  const Command Command::ping = { .id = 0xF1, .len = sizeof(uint8_t) }; // return the sent integer as a debug message
  const Command Command::prog = { .id = 0xF2, .len = sizeof(address_t) }; // enter programming mode if value is the device address (to be extra sure this isn't accidental)

  struct SHITP_Message {
    static constexpr uint8_t max_data_length = 64;

    // Header Struct (standard format for message headers):
    struct {
      union messageHeader_t { // anonymous union
        struct headerData_t {
          address_t address; //      - Bus Address of Device being Targeted
          Command command; //        - Command being issued
        } data;
        uint8_t raw[sizeof(headerData_t)];
      };
    } header;

    uint8_t data[max_data_length];
  };

 class SHITP_CommsLine {
  public:
    static constexpr char messageInitiator = '#';
    SHITP_Message message; // Contains actively handled message (persistent object helps fight fragmentation)
    void (*interpreter)(Command, uint8_t*); // Interpreter function to call once a new message has been loaded (takes in command and pointer to message data)

    // Stream over which communication occurs (note, this does not necessarily
    // line up with an RS-485 line, eg. on an M1C1v2 there is the primary
    // hardware USART stream has its read line on one RS-485 bus and the write
    // line on another).
    Stream stream;
    address_t* deviceAddress; // Address of this device (pointer so that it can be stored elsewhere in memory)
    uint8_t pin_re; // Pin which activates the read line (active-low assumed).
    uint8_t pin_de; // Pin which activates the transmit line.
    long baud; // Assumed baud rate for the Serial stream.

    SHITP_CommsLine(Stream& s, address_t* addr, uint8_t re, uint8_t de, long b, void (*interp)(Command, uint8_t*)){
      stream = s;
      deviceAddress = addr;
      pin_re = re;
      pin_de = de;
      baud = b;
      interpreter = interp;
    }

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
      stream.write(messageInitiator);
      stream.write(b);
      stream.flush(); // wait for all currently staged outgoing data to finish sending
      setWrite(false);
    }
    // Writes the given buffer with the given length to the line
    void write(uint8_t* buff, const uint8_t len){
      setWrite(true);
      stream.write(messageInitiator);
      stream.write(buff, len);
      stream.flush(); // wait for all currently staged outgoing data to finish sending
      setWrite(false);
    }
    // Writes the given message to the line.
    void write(SHITP_Message& msg){
      setWrite(true);
      stream.write(messageInitiator);
      stream.write(msg.header.raw, sizeof(msg.header.messageHeader_t));
      stream.write(msg.data, msg.header.command.len);
      stream.flush(); // wait for all currently staged outgoing data to finish sending
      setWrite(false);
    }

    // Performs a comms update (really bad bc it's blocking. Actual M1 Comms is
    // non-blocking.)
    void update(){
      static size_t size = 0;

      // Read in new message:
      if(stream.available() > 0){
        // Look for initializer:
        while(char(stream.read()) != messageInitiator){};
        // Load header:
        size = stream.readBytes(message.header.raw, sizeof(message.header.headerData_t));
        if(
          size == sizeof(message.header.headerData_t) // all header data loaded successfully
          && message.header.address == *deviceAddress // and this device is being addressed
        ){
          // Read message bytes into data:
          size = stream.readBytes(message.data, message.header.command.len);
          if(size == size_t(message.header.command.len)){ // all message data loaded successfully
            interpreter(message.header.command, message.data); // Call the interpreter
          }
        }
      }
    }
 };

#endif //SHIPT_COMMS_H
