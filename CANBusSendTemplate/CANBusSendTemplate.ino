#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10); // CS pin for MCP2515
int counter = 0; // Counter to send as part of the test data

/*
  Program sends data over CAN bus
  NOTE: This is meant as a template for general use
        Check the "NOTE" comments within loop() to see what can be changed
  Written by: Frank Rossi 2024-2025        
*/
void setup() {
  Serial.begin(115200);

  // Initialize MCP2515
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  Serial.println("CAN Bus Test: Sending Arbitrary Data");
}

void loop() {
  /*
    NOTE: data needs to be converted to bytes as HEX and stored in the canMsg.data array
          This can be done by converting it using pointers or manually setting it 
          (as well as other ways you can figure out yourself if needed)

          Example of both are shown below, make sure to keep one of the two commented out when testing
  */

  // Prepare the CAN message
  canMsg.can_id = 0x123; // Example CAN ID (24-25 ID List can be found in the frontBox repo readMe)

  //NOTE: canMsg.can_dlc = sizeof(*VariableBeingSent*) also works if the size will change
  canMsg.can_dlc = 8;    // Data Length Code (8 bytes max)

  //NOTE: This works for any data type not only doubles, you just can't send more than 8 bytes at a time
  double valueOne = 103.8972 + counter;
  double valueTwo = 21.00812 + counter;
  /*NOTE: Knowing what the pointers are doing is unecessary
          Just keep the data in the following format
          nth value: *(DataType *) &canMsg.data[i] = valueN; (i = the starting byte of the value within data)
  */
  *(double *) &canMsg.data[0] = valueOne; //double is 4 bytes on most arduinos, instead of 8
  *(double *) &canMsg.data[4] = valueTwo; 


  //NOTE: I found this useful while getting canBus working but it is a less effective method
  // canMsg.data[0] = 0xAA; // Example bytes
  // canMsg.data[1] = 0xBB; 
  // canMsg.data[2] = counter >> 8;  // High byte of counter
  // canMsg.data[3] = counter & 0xFF; // Low byte of counter
  // canMsg.data[4] = 0xCC;
  // canMsg.data[5] = 0xDD; 
  // canMsg.data[6] = 0xEE; 
  // canMsg.data[7] = 0xFF; 

  // Send the CAN message
  mcp2515.sendMessage(&canMsg);

  // Print debug info to Serial Monitor
  Serial.print("Sent CAN ID: 0x");
  Serial.print(canMsg.can_id, HEX);
  Serial.print(" Data: ");
  for (int i = 0; i < canMsg.can_dlc; i++) {
    Serial.print(canMsg.data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  //NOTE: Data can also be printed as a double using pointers
  // Serial.println((*(double*)canMsg.data), 6);
  // Serial.println((*(double*)&canMsg.data[4]), 6);

  // Increment the counter and wrap around after 65535
  counter = (counter + 1) % 65536;

  delay(100); // Delay to avoid flooding the CAN bus
}
