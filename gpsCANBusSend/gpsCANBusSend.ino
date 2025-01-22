#include <SPI.h>
#include <mcp2515.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

struct can_frame canMsg;
MCP2515 mcp2515(10); // CS pin for MCP2515

SoftwareSerial gpsSerial(8, 9); // RX = 8, TX Unused
TinyGPSPlus gps;

double longitude = 0;
double latitude = 0;
double altitude = 0
double  speed = 0;
uint32_t time = 0;
uint32_t date = 0;

/*
  Program Uses a GPS module and an MCP2515 to send GPS data over CAN bus
  GPS  - RX goes to pin 8, TX is unused
       - 3.3 and 5v work
  MCP2515 - ....
  Written by: Frank Rossi 2024-2025
*/

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("GPS Module Initialized");
  // Initialize MCP2515
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  Serial.println("Setup Complete");
}

//Function converts two doubles to 4 byte hex and sends them over CAN bus
//NOTE: Doubles are 4 bytes in arduino uno
void sendTwoDoubles(uint32_t id, double valueOne, double valueTwo) {
  canMsg.can_id = id; 

  //Length of message is always 8 bytes
  canMsg.can_dlc = 8;

  //pointer magic (get values as 4 byte arrays)
  *(double *) canMsg.data = valueOne;
  *(double *) &canMsg.data[4] = valueTwo;
  mcp2515.sendMessage(&canMsg);


  //Uncomment to print sent messages to serial monitor
  //debugSend();
}
//Function converts two uint32_ts to 4 byte hex and sends them over CAN bus
void sendTwoUINTs(uint32_t id, uint32_t valueOne, uint32_t valueTwo) {
  canMsg.can_id = id; 

  //Length of message is always 8 bytes
  canMsg.can_dlc = 8;

  //pointer magic (get values as 4 byte arrays)
  *(uint32_t *) canMsg.data = valueOne;
  *(uint32_t *) &canMsg.data[4] = valueTwo;

  mcp2515.sendMessage(&canMsg);

  //Uncomment to print sent messages to serial monitor
  //debugSend();
}


void loop() {
  //Constantly update and send data over CAN bus if available
  while (gpsSerial.available() > 0) {
    char gpsChar = gpsSerial.read();
    if (gps.encode(gpsChar)) {
      // Send GPS data over CAN bus
      sendGPSInfo();
    }
  }
}

//Function sends data over CAN bus if it is updated
void sendGPSInfo() {
  //INFO: function takes 5-9 ms to execute

  // Check if location data is available
  if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
    //Get updated gps data 
    //NOTE: gps call could probably be done within the function calls but I havent tested it
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    speed = gps.speed.kmph();
    altitude = gps.altitude.meters();
    time = gps.time.value();
    date = gps.date.value();

    // Send gps data
    sendTwoDoubles(0x111, latitude, longitude);
    sendTwoDoubles(0x121, speed, altitude);
    sendTwoUINTs(0x131, time, date);
  }
}

// DEBUG Function: prints current canMsg data to serial monitor
//Untested: if it doesnt work paste it all at the end of the sendDouble and sendUINT functions
void debugSend() {
  
  Serial.print("Sent CAN ID: 0x");
  Serial.print(canMsg.can_id, HEX);
  Serial.print(" Data: ");
  for (int i = 0; i < canMsg.can_dlc; i++) {
    Serial.print(canMsg.data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  //Uint32 to double conversion should be fine, it is untested tho so change it if it doesnt work
  Serial.println((*(double*)canMsg.data), 6);
  Serial.println((*(double*)&canMsg.data[4]), 6);

}
