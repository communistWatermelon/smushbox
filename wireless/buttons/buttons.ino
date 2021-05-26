#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

#define MILLIDEBOUNCE 1 //Debounce time in milliseconds
#define pinOBLED 21  //Onboard LED pin

byte buttonStatus[16] = { 0 };
const byte startMarkerValue = 2;
const byte endMarkerValue = 3;

const unsigned long sendInterval = 50;
unsigned long previousSendTime = 0;

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
Bounce buttonLT = Bounce();
Bounce buttonRT = Bounce();
Bounce buttonModUp = Bounce();
Bounce buttonModDown = Bounce();
Bounce cstickUP = Bounce();
Bounce cstickDOWN = Bounce();
Bounce cstickLEFT = Bounce();
Bounce cstickRIGHT = Bounce();

const uint64_t address = 0xF0F0F0F0E1LL;

void setupPins(){
  joystickUP.attach(5,INPUT_PULLUP);
  joystickDOWN.attach(3,INPUT_PULLUP);
  joystickLEFT.attach(4,INPUT_PULLUP);
  joystickRIGHT.attach(2,INPUT_PULLUP);
  cstickUP.attach(19,INPUT_PULLUP);
  cstickDOWN.attach(20,INPUT_PULLUP);
  cstickLEFT.attach(9,INPUT_PULLUP);
  cstickRIGHT.attach(18,INPUT_PULLUP);
  buttonA.attach(21,INPUT_PULLUP);
  buttonB.attach(15,INPUT_PULLUP);;
  buttonY.attach(10,INPUT_PULLUP);
  buttonRB.attach(16,INPUT_PULLUP);
  buttonLT.attach(8,INPUT_PULLUP);
  buttonRT.attach(14,INPUT_PULLUP);
  buttonModUp.attach(6,INPUT_PULLUP);
  buttonModDown.attach(7,INPUT_PULLUP);

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
  buttonModUp.interval(MILLIDEBOUNCE);
  buttonModDown.interval(MILLIDEBOUNCE);
      
  pinMode(pinOBLED, OUTPUT);  
  //Set the LED to low to make sure it is off
  digitalWrite(pinOBLED, HIGH);
}
void setup() {
  setupPins();
//  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop() {
  buttonRead();
  // put your main code here, to run repeatedly:
  sendData();/* Printing POT value on serial monitor*/
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}

  if (buttonModUp.update()) {buttonStatus[BUTTONMODUP] = buttonModUp.fell();}
  if (buttonModDown.update()) {buttonStatus[BUTTONMODDOWN] = buttonModDown.fell();}

  if (cstickUP.update()) {buttonStatus[CBUTTONUP] = cstickUP.fell();}
  if (cstickDOWN.update()) {buttonStatus[CBUTTONDOWN] = cstickDOWN.fell();}
  if (cstickLEFT.update()) {buttonStatus[CBUTTONLEFT] = cstickLEFT.fell();}
  if (cstickRIGHT.update()) {buttonStatus[CBUTTONRIGHT] = cstickRIGHT.fell();}

  if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}  
  if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}
  if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}
  
  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonLT.update()) {buttonStatus[BUTTONLT] = buttonLT.fell();}
  if (buttonRT.update()) {buttonStatus[BUTTONRT] = buttonRT.fell();}
}

void sendData() {
  byte dataToSend[END_MARKER + 1] = { 0 };
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

//  for (uint8_t i = 0; i < arrLength; i++) {
//    Serial.print(dataToSend[i]);
//  }
//  Serial.println("");
//  Serial.flush();
  
  Serial1.write(dataToSend, arrLength);
  Serial1.flush();
}
