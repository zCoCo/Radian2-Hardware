// TODO: GEAR RATIO, Verify live speed setting (incl. decel), check torque at 2Arms, verify enable/disable, verify multidevice, higher baud, cmd to toggle basestation serial feedback

// Tests if the RS485 Comm Lines work using the Simplified Hefty Information Tranmission protocol by reading a device ID and angle from the computer and telling that actuator to move to the specified angle.
/* Basic Basestation-Computer Communication Protocol:
 * A series of commands each of which is composed of a single character command identifier followed by a number (float or integer depending on the command specified). Valid commands:
 * NXXX     - Set the commanded actuator address number (uint8)
 * Db       - Enable/Disable the Driver of the actuator with the specified address (given a binary digit 1/0)
 * AXXX.XXX - Tell the actuator with the specified address to move to the specified angle in degrees (float)
 * VXXX     - Set the running speed for the actuator with the specified address in degrees/s (uint16)
 * LXXX     - Tell the actuator to set the LED to the specified color (uint8: 0=off, 1=white, 2=error, 3=idle, 4==active)
 * PXXX     - (Ping) Tell the actuator with the specified address to send back the given integer (uint8)
 * R        - Tell the device with the specified address to enter programming mode (for Reprogramming, no args)
 * Basic interface settings for computer to control:
 * _b       - Turn console logging on or off (given a binary digit 1/0)
 * *b       - Turn debug logging (messages sent from devices) on or off (given a binary digit 1/0)
 */

/* Programming Settings:
Board: ESP32 Dev Module
Upload Freq: 921600Hz
CPU Freq: 240MHz (WiFi/BT)
Flash Freq: 80MHz
Flash Mode: QIO
Flash Size: 4MB (32Mb)
Partitioning: 4MB with spiffs (1.2MB APP | 1.5MB SPIFFS)
Core Debug Level: None (change as needed)
PSRAM: Disabled (change as needed)

Uploader: FTDI RS232 3V3
Procedure: Jump J_DBG0 (_RRE_OVERRIDE), Hold B_PROG0, Pulse B_RST0 (Serial will read: Waiting for Download)
*/

#include "shitp_comms.h"

// NB: al_ indicates active low

// -- Onboard IO --
#define FAN 23
#define LED 2
#define WIFI_ENABLE 13

// -- Onboard State Monitoring --
#define VM_SENS 35
#define VL_SENS 32

// -- COMMS --
address_t DEV_ID = 0xFF; // Device ID of the basestation

// UART0 (Primary comm with computer, secondary comm with actuators):
#define TTX 1
#define RRX 3

// UART2 (Primary comm with actuators):
#define RTX 17
#define TRX 16

// Control Lines:
// NOTE: If multiple _RREx are low, two transceivers could try to drive the same line and blow each other up (maybe). Write a wrapper to make sure this doesn't happen.
// Control Lines for Lines Actuators Read on (which you'll want to write to):
#define RDEA 22
#define RDEB 5
#define RDEC 4
#define al_RREA_CTRL 21 // _RREx_CTRL is basically offering a suggested _RREx but can be overridden by circuitry to prevent two transceivers from controlling the line at once (ie, when using the debugging port)
#define al_RREB_CTRL 19
#define al_RREC_CTRL 18

// Control Lines for Lines Actuators Write on (which you'll want to read from):
#define TDE 33 // TDE=TDEA=TDEB=TDEC
#define al_TREA 25
#define al_TREB 26
#define al_TREC 27

// Connection Sensing (3.3V = DISCONNECTED, CONNECTED = 1.65V -> .: CONNECTEDx = CONN_x<2.48V
#define CONN_A 34
#define CONN_B 39
#define CONN_C 36

// Communication Lines:
// Interprets / acts upon the given command with the associated data payload.
bool device_debug_logging = true;
void command_interpreter(Command cmd, uint8_t* data){
  switch(cmd.id){
    case Commands::debug.id:
      if(device_debug_logging){
        Serial.print("\t\t > Received: ");
        Serial.write(data, cmd.len);
        Serial.print('\n');
      }
    break;
  }
}

void init_serial(long baud){
  Serial2.begin(baud, SERIAL_8N1, TRX, RTX);
}

bool enable_logging = true;
void console_log(char* msg){
  if(enable_logging){
    Serial.print(msg);
  }
}

long baud = 115200;
SHITP_CommsLine ChainA(&Serial2, &init_serial, &DEV_ID, al_TREA, RDEA, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainA.
//SHITP_CommsLine ChainA_secondary(Serial2, DEV_ID, al_RREA_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainA.

//SHITP_CommsLine ChainB(&Serial2, &init_serial, &DEV_ID, al_TREB, RDEB, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainB.
//SHITP_CommsLine ChainB_secondary(Serial2, DEV_ID, al_RREB_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainB.

//SHITP_CommsLine ChainC(Serial2, &DEV_ID, al_TREC, RDEC, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainC.
//SHITP_CommsLine ChainC_secondary(Serial2, DEV_ID, al_RREC_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainC.


// setting PWM properties
const int freq = 5000;
const int ledChannel = 0; // ESP32 PWM Channel
const int resolution = 10; // Resolution 8, 10, 12, 15

void setup() {
  Serial.begin(115200);
  console_log("Begin. . .");

  ChainA.begin();

  pinMode(LED, OUTPUT);
  pinMode(WIFI_ENABLE, INPUT);

  // configure "LED" (fan) PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(FAN, ledChannel);
  ledcWrite(ledChannel, 1024); // Set fan power to 100%
}

SHITP_Message msg; // message being built
char str[100];
char c; // character being read
uint8_t color;
ByteUnion<float> floatunion; // utility float union
ByteUnion<uint16_t> u16union; // utility uint16 union
ByteUnion<address_t> addrunion; // utility address union
unsigned long last_blink = 0;
bool led_state = 0;
void loop() {
  // Read
  while(Serial.available() > 0){
    c = Serial.read();
    switch(c){
      // Computer/Console Interface Commands:
      case '_': // Enable/Disable Console Logging from Basestation
        enable_logging = Serial.parseInt();
        Serial.print("Basestation console logging turned ");
        Serial.print(enable_logging ? "on" : "off");
        Serial.println(".");
      break;
      case '*': // Enable/Disable Logging of Debug Messages from Devices
        device_debug_logging = Serial.parseInt();
        Serial.print("Device debug logging turned ");
        Serial.print(device_debug_logging ? "on" : "off");
        Serial.println(".");
      break;
      // Device Interface Commands:
      case 'N': // Set target address
        msg.header.data.address = Serial.parseInt();
        sprintf(str, "Set Target Address to: %d\n", msg.header.data.address);
        console_log(str);
      break;

      case 'A': // Send message to move to given angle
        msg.header.data.command = Commands::move_to;
        floatunion.val = Serial.parseFloat();
        memcpy(msg.data, floatunion.data, sizeof(float));
        sprintf(str, "\t Commanding Device %d to move to %0.3fdeg\n", msg.header.data.address, floatunion.val);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
      case 'V': // Send message specifying speed
        msg.header.data.command = Commands::set_speed;
        u16union.val = Serial.parseInt();
        memcpy(msg.data, u16union.data, sizeof(uint16_t));
        sprintf(str, "\t Commanding Device %d to move at %ddeg/s\n", msg.header.data.address, u16union.val);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
      case 'D': // Disable/Enable Device
        msg.header.data.command = Commands::enable;
        msg.data[0] = Serial.parseInt();
        sprintf(str, msg.data[0] ? "\t Enabling Device %d\n" : "\t Disabling Device %d\n", msg.header.data.address);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
      case 'C': // Complete the Current Move before Accepting a New Setpoint
        msg.header.data.command = Commands::complete_move;
        msg.data[0] = Serial.parseInt();
        sprintf(str, "\t Telling Device %d to complete the given waypoint\n", msg.header.data.address);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
      case 'H': // Hold the Current Setpoint (don't accept a new one) for the Given Number of Milliseconds
        msg.header.data.command = Commands::hold_setpoint;
        u16union.val = Serial.parseInt();
        memcpy(msg.data, u16union.data, sizeof(uint16_t));
        sprintf(str, "\t Telling Device %d to hold the setpoint for %dms \n", msg.header.data.address, u16union.val);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;

      case 'L': // Sets the color to the given color
        msg.header.data.command = Commands::led_color;
        color = Serial.parseInt();
        switch(color){
          case 0:
            memcpy(msg.data, &COLOR::off, sizeof(color_t));
            sprintf(str, "\t Setting LEDs on Device %d to Off\n", msg.header.data.address);
          break;
          case 1:
            memcpy(msg.data, &COLOR::white, sizeof(color_t));
            sprintf(str, "\t Setting LEDs on Device %d to White\n", msg.header.data.address);
          break;
          case 2:
            memcpy(msg.data, &COLOR::error, sizeof(color_t));
            sprintf(str, "\t Setting LEDs on Device %d to Error (Red)\n", msg.header.data.address);
          break;
          case 3:
            memcpy(msg.data, &COLOR::idle, sizeof(color_t));
            sprintf(str, "\t Setting LEDs on Device %d to Idle (Blue)\n", msg.header.data.address);
          break;
          case 4:
            memcpy(msg.data, &COLOR::active, sizeof(color_t));
            sprintf(str, "\t Setting LEDs on Device %d to Active (Green)\n", msg.header.data.address);
          break;
        }
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;

      case 'P': // Ping the actuator, ask for the integer back as a debug message.
        msg.header.data.command = Commands::ping;
        msg.data[0] = Serial.parseInt();
        sprintf(str, "\t Pinging Device %d with %d\n", msg.header.data.address, msg.data[0]);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
      case 'R': // Tell the actuator to enter programming mode
        msg.header.data.command = Commands::prog;
        addrunion.val = msg.header.data.address;
        memcpy(msg.data, addrunion.data, sizeof(address_t));
        sprintf(str, "\t Putting Device %d into programming mode\n", msg.header.data.address);
        console_log(str);
        ChainA.write(msg);
        console_log("\t\t > Sent.\n");
      break;
    }
  }

  ChainA.update(); // Read data from the line

  if((millis() - last_blink) >= 1000){
    led_state = !led_state;
    digitalWrite(LED, led_state);
    last_blink = millis();
  }
}
