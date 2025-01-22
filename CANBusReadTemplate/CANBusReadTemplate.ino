#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10); // CS Pin for MCP2515

/*
  Program reads CANBUS data and prints it to serial
  NOTE: This is meant as a template for general use
        Check the "NOTE" comments within the loop to see what can be changed
  Written by: Frank Rossi 2024-2025
*/
void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.println();
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC (# of bytes in message)
    Serial.print(" ");

    //NOTE: Change the data type to whatever is needed
    //If only reading 4 bytes the second print statement can be commented out 
    Serial.print((*(double*)canMsg.data), 6); //second number is # of decimal places
    Serial.print(*(double*)&canMsg.data[4], 6);

    /*NOTE #2: if you need the bytes and not nums you can use the loop below*/
    // for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
    //   Serial.print(canMsg.data[i], HEX);
    //   Serial.print(" ");
    // }
    //Serial.println();
  }
}
