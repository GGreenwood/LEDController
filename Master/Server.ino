#include <SPI.h>
#include <AIR430BoostFCC.h>

// -----------------------------------------------------------------------------
/**
 *  Global data
 */

uint8_t deviceID = 0;

// Data to read from radio RX FIFO (60 bytes MAX.)
unsigned char rxData[5] = { '\0', '\0' , '\0', '\0', '\0'};

// Amount of time to wait for an echo before resending
uint32_t delayAmount = 100;

// Number of times to retry sending
uint8_t tries = 20;

// -----------------------------------------------------------------------------
// Main example

void setup()
{
  // The radio library uses the SPI library internally, this call initializes
  // SPI/CSn and GDO0 lines. Also setup initial address, channel, and TX power.
  Radio.begin(0x01, CHANNEL_4, POWER_MAX);

  // Setup serial for debug printing.
  Serial.begin(9600);
  
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);   // set the LED off
  Serial.println("Initialized");
}

void loop()
{
  if(Serial.available() > 0) {
    char arg[6];
    Serial.readBytes(arg, 6);
    //send(1, 1, 50);
    send(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
  }
}



boolean send(uint8_t device, uint8_t mode, uint8_t wait, uint8_t red, uint8_t green, uint8_t blue) {
  Serial.println("Sending packet");
  uint8_t txData[7] = {device, mode, wait, red, green, blue, '\0'};   
  digitalWrite(RED_LED, HIGH);   // set the LED on
  //txData[1] = ++txData[1] % 2;
  //txData[2];
  for(uint8_t i = 0; i < tries; i++) {
    Radio.transmit(ADDRESS_BROADCAST, txData , 7);
    while (Radio.busy());
    if (Radio.receiverOn(rxData, sizeof(rxData), delayAmount) > 0) {
      // Target needs to be 
      if(rxData[0] == deviceID && rxData[1] == 0 && rxData[2] == device) {
        Serial.println("Echo received, stopping");
        break;
      }
    }
    String msg = "Echo not received, trying again";
    msg.concat(rxData[0]);
    msg.concat(rxData[1]);
    msg.concat(rxData[2]);
    msg.concat(device);
    Serial.println(msg);
  }
  digitalWrite(RED_LED, LOW);   // set the LED off
}
  
