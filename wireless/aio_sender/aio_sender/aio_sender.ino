#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>
#include <SPI.h>                  /* to handle the communication interface with the modem*/
#include <nRF24L01.h>             /* to handle this particular modem driver*/
#include <RF24.h>                 /* the library which helps us to control the radio modem*/
#include <printf.h>

#define MILLIDEBOUNCE 1 //Debounce time in milliseconds

byte buttonStatus[18] = { 0 };
const byte startMarkerValue = 2;
const byte endMarkerValue = 3;

unsigned long previousSendTime = 0;

bool debugMode = false;
#define CE_PIN   10
#define CSN_PIN 9

RF24 radio(CSN_PIN, CE_PIN);
const uint64_t address = 0xF0F0F0F0E1LL;

const unsigned long sendInterval = 5;

#define START_MARKER 0
#define BUTTONUP 1
#define BUTTONDOWN 2
#define BUTTONLEFT 3
#define BUTTONRIGHT 4
#define BUTTONA 5
#define BUTTONB 6
#define BUTTONY 7
#define BUTTONRB 8
#define BUTTONLT 9
#define BUTTONRT 10
#define CBUTTONUP 11
#define CBUTTONDOWN 12
#define CBUTTONLEFT 13
#define CBUTTONRIGHT 14
#define BUTTONMODUP 15
#define BUTTONMODDOWN 16
#define END_MARKER 17

Bounce joystickUP = Bounce();
Bounce joystickDOWN = Bounce();
Bounce joystickLEFT = Bounce();
Bounce joystickRIGHT = Bounce();
Bounce buttonA = Bounce();
Bounce buttonB = Bounce();
Bounce buttonY = Bounce();
Bounce buttonRB = Bounce();
Bounce buttonRT = Bounce();
Bounce buttonLT = Bounce();
Bounce buttonModDown = Bounce();
Bounce cstickUP = Bounce();
Bounce cstickDOWN = Bounce();
Bounce cstickLEFT = Bounce();
Bounce cstickRIGHT = Bounce();

// can't use 9, 10, 11, 12, 13
void setupPins(){
  joystickUP.attach(7,INPUT_PULLUP);
  joystickDOWN.attach(8,INPUT_PULLUP);
  joystickLEFT.attach(6,INPUT_PULLUP);
  joystickRIGHT.attach(5,INPUT_PULLUP);
  cstickUP.attach(4,INPUT_PULLUP); 
  cstickDOWN.attach(3,INPUT_PULLUP); 
  cstickLEFT.attach(2,INPUT_PULLUP); 
  cstickRIGHT.attach(A0,INPUT_PULLUP); 
  buttonA.attach(A1,INPUT_PULLUP);
  buttonB.attach(A2,INPUT_PULLUP); 
  buttonY.attach(A3,INPUT_PULLUP);
  buttonRB.attach(A4,INPUT_PULLUP);
  buttonLT.attach(A5,INPUT_PULLUP);
  buttonRT.attach(1,INPUT_PULLUP);
  buttonModDown.attach(0,INPUT_PULLUP);

  joystickUP.interval(MILLIDEBOUNCE);
  joystickDOWN.interval(MILLIDEBOUNCE);
  joystickLEFT.interval(MILLIDEBOUNCE);
  joystickRIGHT.interval(MILLIDEBOUNCE);
  cstickUP.interval(MILLIDEBOUNCE);
  cstickDOWN.interval(MILLIDEBOUNCE);
  cstickLEFT.interval(MILLIDEBOUNCE);
  cstickRIGHT.interval(MILLIDEBOUNCE);
  buttonA.interval(MILLIDEBOUNCE);
  buttonB.interval(MILLIDEBOUNCE);
  buttonY.interval(MILLIDEBOUNCE);
  buttonRB.interval(MILLIDEBOUNCE);
  buttonLT.interval(MILLIDEBOUNCE);
  buttonRT.interval(MILLIDEBOUNCE);
  buttonModDown.interval(MILLIDEBOUNCE);
}
void setup() {
  if (debugMode) {
    Serial.begin(9600);
    printf_begin();
  }
  radio.begin(); /* Activate the modem*/
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.powerUp();
  radio.openWritingPipe(address); /* Sets the address of transmitter to which program will send the data */
  radio.stopListening(); 
  setupPins();
  
}

void loop() {
  buttonRead();
  // put your main code here, to run repeatedly:
  sendData();/* Printing POT value on serial monitor*/
}

byte readAnalogValue(int pin) {
  int total = 0;
  for (uint8_t i = 0; i < 5; i++) {
    total += analogRead(pin);
  }
  return total == 0 ? 1 : 0;
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}

  buttonStatus[BUTTONMODUP] = readAnalogValue(A7);
  if (buttonModDown.update()) {buttonStatus[BUTTONMODDOWN] = buttonModDown.fell();}

  if (cstickUP.update()) { buttonStatus[CBUTTONUP] = cstickUP.fell(); }
  if (cstickDOWN.update()) { buttonStatus[CBUTTONDOWN] = cstickDOWN.fell(); }
  if (cstickLEFT.update()) { buttonStatus[CBUTTONLEFT] = cstickLEFT.fell(); }
  if (cstickRIGHT.update()) { buttonStatus[CBUTTONRIGHT] = cstickRIGHT.fell(); }

  if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}  
  if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}
  if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}
  
  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonLT.update()) {buttonStatus[BUTTONLT] = buttonLT.fell();}
  if (buttonRT.update()) {buttonStatus[BUTTONRT] = buttonRT.fell();}
}

void debugPrint(String line, int button) {
  if (!debugMode || !buttonStatus[button]) return;
  Serial.println(line);
}

void sendData() {
  unsigned long currentTime = millis();
  
  byte dataToSend[18] = { 0 };
  byte arrLength = sizeof(dataToSend) / sizeof(dataToSend[0]);

  dataToSend[START_MARKER] = 2;
  dataToSend[BUTTONUP] = buttonStatus[BUTTONUP] ? 1 : 0;
  dataToSend[BUTTONDOWN] = buttonStatus[BUTTONDOWN] ? 1 : 0;
  dataToSend[BUTTONLEFT] = buttonStatus[BUTTONLEFT] ? 1 : 0;
  dataToSend[BUTTONRIGHT] = buttonStatus[BUTTONRIGHT] ? 1 : 0;
  dataToSend[BUTTONA] = buttonStatus[BUTTONA] ? 1 : 0;
  dataToSend[BUTTONB] = buttonStatus[BUTTONB] ? 1 : 0;
  dataToSend[BUTTONY] = buttonStatus[BUTTONY] ? 1 : 0;
  dataToSend[BUTTONRB] = buttonStatus[BUTTONRB] ? 1 : 0;
  dataToSend[BUTTONLT] = buttonStatus[BUTTONLT] ? 1 : 0;
  dataToSend[BUTTONRT] = buttonStatus[BUTTONRT] ? 1 : 0;
  dataToSend[CBUTTONUP] = buttonStatus[CBUTTONUP] ? 1 : 0;
  dataToSend[CBUTTONDOWN] = buttonStatus[CBUTTONDOWN] ? 1 : 0;
  dataToSend[CBUTTONLEFT] = buttonStatus[CBUTTONLEFT] ? 1 : 0;
  dataToSend[CBUTTONRIGHT] = buttonStatus[CBUTTONRIGHT] ? 1 : 0;
  dataToSend[BUTTONMODUP] = buttonStatus[BUTTONMODUP] ? 1 : 0;
  dataToSend[BUTTONMODDOWN] = buttonStatus[BUTTONMODDOWN] ? 1 : 0;
  dataToSend[END_MARKER] = endMarkerValue;

  debugPrint("BUTTONUP", BUTTONUP);
  debugPrint("BUTTONDOWN", BUTTONDOWN);
  debugPrint("BUTTONLEFT", BUTTONLEFT);
  debugPrint("BUTTONRIGHT", BUTTONRIGHT);
  debugPrint("BUTTONA", BUTTONA);
  debugPrint("BUTTONB", BUTTONB);
  debugPrint("BUTTONY", BUTTONY);
  debugPrint("BUTTONRB", BUTTONRB);
  debugPrint("BUTTONLT", BUTTONLT);
  debugPrint("BUTTONRT", BUTTONRT);
  debugPrint("CBUTTONUP", CBUTTONUP);
  debugPrint("CBUTTONDOWN", CBUTTONDOWN);
  debugPrint("CBUTTONLEFT", CBUTTONLEFT);
  debugPrint("CBUTTONRIGHT", CBUTTONRIGHT);
  debugPrint("BUTTONMODUP", BUTTONMODUP);
  debugPrint("BUTTONMODDOWN", BUTTONMODDOWN);

  if (currentTime - previousSendTime >= sendInterval) { 
    if (debugMode) {
      for (uint8_t i = 0; i < arrLength; i++) {
        Serial.print(dataToSend[i]);
      }
      Serial.println("");
      Serial.flush();
    }
    radio.write(dataToSend, sizeof(dataToSend)); /* Sending data over NRF 24L01*/
    radio.txStandBy(sendInterval);
    previousSendTime = currentTime;
  }
}
