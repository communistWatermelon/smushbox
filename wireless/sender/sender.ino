#include <SPI.h>                  /* to handle the communication interface with the modem*/
#include <nRF24L01.h>             /* to handle this particular modem driver*/
#include <RF24.h>                 /* the library which helps us to control the radio modem*/
#include <printf.h>

#define pinOBLED 21  //Onboard LED pin

RF24 radio(3, 4);
const uint64_t address = 0xF0F0F0F0E1LL;
const byte readLength = 18;

const byte endMarkerValue = 3;

uint32_t previousData; 

bool debugMode = false;

const unsigned long sendInterval = 5;
unsigned long previousSendTime = 0;

void setupPins(){
  pinMode(pinOBLED, OUTPUT);  
  //Set the LED to low to make sure it is off
  digitalWrite(pinOBLED, HIGH);
}

void setup() {
  setupPins();
  if (debugMode) {
    printf_begin();
    Serial.begin(9600);
  }
  Serial1.begin(115200);
  radio.begin(); /* Activate the modem*/
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.powerUp();
  radio.openWritingPipe(address); /* Sets the address of transmitter to which program will send the data */
  radio.stopListening(); 
}

void loop() {
  unsigned long currentTime = millis();
  
  if (Serial1.available() > 0) {
    byte dataToSend[readLength] = { 0 };
    byte temp[1] = { 0 };
    
    boolean foundEndMarker = false;
    while (!foundEndMarker) {
      Serial1.readBytes(temp, 1);
      foundEndMarker = temp[0] == endMarkerValue;
    }
    Serial1.readBytes(dataToSend, readLength);

    if (debugMode) {
      for (uint8_t i = 0; i < readLength; i++) {
        Serial.print(dataToSend[i]);
      }
      Serial.println("");
      Serial.flush();
    }
      
    if (currentTime - previousSendTime >= sendInterval) { 
      radio.write(dataToSend, readLength); /* Sending data over NRF 24L01*/
      radio.txStandBy(1000);
      previousSendTime = currentTime;
    }
  }
}
