// Tests if the RS485 Comm Lines work using the Simplified Hefty Information Tranmission protocol by reading a device ID and angle from the computer and telling that actuator to move to the specified angle.
/* Basic Basestation-Computer Communication Protocol:
 * A series of commands each of which is composed of a single character command identifier followed by a number (float or integer depending on the command specified). Valid commands:
 * NXXX     - Set the commanded actuator address number (uint8)
 * AXXX.XXX - Tell the actuator with the specified address to move to the specified angle in degrees (float)
 * VXXX     - Set the running speed for the actuator with the specified address in degrees/s (uint16)
 * LXXX     - Tell the actuator to set the LED to the specified color (uint8: 0=off, 1=white, 2=error, 3=idle, 4==active)
 * PXXX     - (Ping) Tell the actuator with the specified address to send back the given integer (uint8)
 * R        - Tell the device with the specified address to enter programming mode (for Reprogramming, no args)
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
void command_interpreter(Command cmd, uint8_t* data){
  switch(cmd){
    case COMMANDS.debug:
      Serial1.print("Response: ");
      Serial1.write(data, cmd.len);
      Serial1.print('\n');
    break;
  }
}

long baud = 9600;
SHITP_CommsLine ChainA(Serial2, &DEV_ID, al_TREA, RDEA, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainA.
//SHITP_CommsLine ChainA_secondary(Serial2, DEV_ID, al_RREA_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainA.

//SHITP_CommsLine ChainB(Serial2, &DEV_ID, al_TREB, RDEB, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainB.
//SHITP_CommsLine ChainB_secondary(Serial2, DEV_ID, al_RREB_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainB.

//SHITP_CommsLine ChainC(Serial2, &DEV_ID, al_TREC, RDEC, baud, &command_interpreter); // Read from the line the devices transmit on and write to the line the devices read on in ChainC.
//SHITP_CommsLine ChainC_secondary(Serial2, DEV_ID, al_RREC_CTRL, TDE, baud, &command_interpreter); // Read from the line the devices read on and write to the line the devices write on in ChainC.


// setting PWM properties
const int freq = 5000;
const int ledChannel = 0; // ESP32 PWM Channel
const int resolution = 10; //Resolution 8, 10, 12, 15

void setup() {
  Serial1.begin(115200);
  Serial1.println("Begin. . .");

  ChainA.begin();
  
  pinMode(LED, OUTPUT);
  pinMode(WIFI_ENABLE, INPUT);

  digitalWrite(LED, digitalRead(WIFI_ENABLE));

  // configure "LED" (fan) PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN, ledChannel);
}

SHITP_Message msg; // message being built
char c; // character being read
void loop() {
  // Read
  while(Serial1.available() > 0){
    c = Serial1.read();
    switch(c){
      case 'N': // Set target address
        msg.header.address = Serial1.parseInt();
      break;
      case 'A': // Send message to move to given angle
        msg.header.command = Command::move_to;
        ByteUnion<float> angle = { .val = Serial1.parseFloat() };
        memcpy(msg.data, angle.data, sizeof(float));
        ChainA.write(msg);
      break;
      case 'V': // Send message specifying speed
        msg.header.command = Command::set_speed;
        ByteUnion<uint16_t> spd = { .val = Serial1.parseInt() };
        memcpy(msg.data, spd.data, sizeof(uint16_t));
        ChainA.write(msg);
      break;
      case 'L': // Sets the color to the given color
        msg.header.command = Command::led_color;
        uint8_t color = Serial1.parseInt();
        switch(color){
          case 0:
            memcpy(msg.data, color_off, sizeof(COLOR));
          break;
          case 1:
            memcpy(msg.data, color_white, sizeof(COLOR));
          break;
          case 2:
            memcpy(msg.data, color_error, sizeof(COLOR));
          break;
          case 3:
            memcpy(msg.data, color_idle, sizeof(COLOR));
          break;
          case 4:
            memcpy(msg.data, color_active, sizeof(COLOR));
          break;
        }
        ChainA.write(msg);
      break;
      case 'P': // Ping the actuator, ask for the integer back as a debug message.
        msg.header.command = Command::ping;
        msg.data[0] = Serial1.parseInt();
        ChainA.write(msg);
      break;
      case 'R': // Tell the actuator to enter programming mode
        msg.header.command = Command::prog;
        ByteUnion<address_t> addr = { .val = msg.header.address };
        memcpy(msg.data, addr.data, sizeof(address_t));
        ChainA.write(msg);
      break;
    }
  }

  ChainA.update(); // Read data from the line
  
  digitalWrite(LED, digitalRead(WIFI_ENABLE));
}
