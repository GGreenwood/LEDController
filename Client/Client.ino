// The AIR430BoostFCC library uses the SPI library internally. Energia does not
// copy the library to the output folder unless it is referenced here.
// The order of includes is also important due to this fact.
#include <SPI.h>
#include <AIR430BoostFCC.h>

#include <WS2811Driver.h>
#include "lookup_table.h"

// -----------------------------------------------------------------------------
/**
 *  Global data
 */
 
// Setup 150 GRB LED Strip on Launchpad pin P1_6
//const uint8_t NUM_PIXELS = 150;
//WS2811Driver ledStrip = WS2811Driver(NUM_PIXELS, P1_6, NEO_GRB);
//uint8_t deviceID = 1;

// Setup 60 GRB LED Strip on Launchpad pin P1_0
//const uint8_t NUM_PIXELS = 30;
//WS2811Driver ledStrip = WS2811Driver(30, P1_0, NEO_GRB);
//uint8_t deviceID = 2;

// Setup 60 GRB LED Strip on Launchpad pin P1_0
const uint8_t NUM_PIXELS = 30;
WS2811Driver ledStrip = WS2811Driver(30, P1_0, NEO_GRB);
uint8_t deviceID = 3;



// Device type
uint8_t deviceType = 1; // LED Strip

// Animation state
uint8_t state = 2;

// Amount to delay
uint8_t delayAmount = 15;

// Color
uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;
uint32_t color = ledStrip.Color(255,255,255);

// Data to write to radio TX FIFO (60 bytes MAX.)
unsigned char txData[5] = {0, 0, deviceID, deviceType, '\0'};    

// Data to read from radio RX FIFO (60 bytes MAX.)
unsigned char rxData[7] = { '\0', '\0' , '\0', '\0', '\0', '\0', '\0'};

// -----------------------------------------------------------------------------
// Main example

void setup()
{
  // The radio library uses the SPI library internally, this call initializes
  // SPI/CSn and GDO0 lines. Also setup initial address, channel, and TX power.
  Radio.begin(0x01, CHANNEL_4, POWER_MAX);

  // Setup serial for debug printing.
  //Serial.begin(9600);
  
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);   // set the LED on
  
  // Setup LED Strip
  ledStrip.setBrightness(255);      //Set LED strip brightness to max
  
  ledStrip.begin();        // configure P1.6 for output
  clearStrip();
  
  //String msg = "Client ";
  //msg.concat(deviceID);
  //msg += " initialized";
  //Serial.println(msg);
  //Serial.println(lookup[5]);
}

void loop()
{
  
  // Turn on the receiver and listen for incoming data. Timeout after 1 seconds.
  // The receiverOn() method returns the number of bytes copied to rxData.
  if (Radio.receiverOn(rxData, sizeof(rxData), delayAmount) > 0)
  {
    /*
    String msg = "Received!";
    msg.concat(rxData[0]);
    msg += " ";
    msg.concat(rxData[1]);
    msg += " ";
    msg.concat(rxData[2]);
    msg += " ";
    Serial.println(msg);
    */
    digitalWrite(RED_LED, HIGH);
    
    if(rxData[0] == deviceID)
    {
      state = rxData[1];

      
      if(rxData[2] > 10) {
        delayAmount = rxData[2];
      } else {
        delayAmount = 10;
      }
      
      red = rxData[3];
      green = rxData[4];
      blue = rxData[5];
      
      color = ledStrip.Color(red, green, blue);
      
      clearStrip();
      
      while (Radio.busy());
      Radio.transmit(ADDRESS_BROADCAST, txData , 5);
      
    }
    digitalWrite(RED_LED, LOW);
  }
  if(state == 0) {
    clearStrip();
  } else if(state == 1) {
    colorWipe(); 
  } else if(state == 2) {
    chaser();
  } else if(state == 3) {
    //stars();
  } else {
    state = 0;
  }
}

uint16_t curPixel = 0;
void colorWipe() {
  ledStrip.setPixelColor(curPixel, color);
  ledStrip.show();
  
  if(++curPixel > NUM_PIXELS) {
    curPixel = 0;
    clearStrip();
  }
}

uint8_t chaserFade[10] = {80, 255, 180, 140, 110, 80, 60, 50, 40, 0};
void chaser() {
  for(uint8_t i = 0; i < sizeof(chaserFade); i++) {
    ledStrip.setPixelColor((curPixel - i) % NUM_PIXELS, ledStrip.Color(chaserFade[i] * red / 255, chaserFade[i] * green / 255, chaserFade[i] * blue / 255));
  }
  curPixel++;
  ledStrip.show();
}

/*
uint8_t stars_buffer[NUM_PIXELS];
void stars() {
  if(random(100) > delayAmount) {
    uint8_t i = random(NUM_PIXELS);
    if(stars_buffer[i] == 0) {
      stars_buffer[i] = 1;
    }
  }
  
  for(uint8_t i = 0; i < NUM_PIXELS; i++) {
    if(stars_buffer[i] > 0) {
      stars_buffer[i]++;
    }
    Serial.print(stars_buffer[i]);
    ledStrip.setPixelColor(i, stars_buffer[i], stars_buffer[i], stars_buffer[i]);
  }
  Serial.println();
  
  ledStrip.show();
}*/


void clearStrip() {
  for(uint8_t i=0; i<NUM_PIXELS; i++)
  {
    ledStrip.setPixelColor(i, 0, 0, 0);
  }
  ledStrip.show();
}
  
