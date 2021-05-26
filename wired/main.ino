#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

#define MILLIDEBOUNCE 1 //Debounce time in milliseconds
#define pinOBLED 21  //Onboard LED pin

const uint8_t minStickValue = 0;
const uint8_t maxStickValue = 255;

byte buttonStatus[27];

#define R3_MASK_ON 0x800
#define L3_MASK_ON 0x400
#define DPAD_UP_MASK_ON 0x00
#define DPAD_UPRIGHT_MASK_ON 0x01
#define DPAD_RIGHT_MASK_ON 0x02
#define DPAD_DOWNRIGHT_MASK_ON 0x03
#define DPAD_DOWN_MASK_ON 0x04
#define DPAD_DOWNLEFT_MASK_ON 0x05
#define DPAD_LEFT_MASK_ON 0x06
#define DPAD_UPLEFT_MASK_ON 0x07
#define DPAD_NOTHING_MASK_ON 0x08
#define A_MASK_ON 0x04
#define B_MASK_ON 0x02
#define X_MASK_ON 0x08
#define Y_MASK_ON 0x01
#define LB_MASK_ON 0x10
#define RB_MASK_ON 0x20
#define ZL_MASK_ON 0x40
#define ZR_MASK_ON 0x80
#define START_MASK_ON 0x200
#define SELECT_MASK_ON 0x100
#define HOME_MASK_ON 0x1000

#define BUTTONUP 0
#define BUTTONDOWN 1
#define BUTTONLEFT 2
#define BUTTONRIGHT 3
#define BUTTONA 4
#define BUTTONB 5
#define BUTTONX 6
#define BUTTONY 7
#define BUTTONLB 8
#define BUTTONRB 9
#define BUTTONLT 10
#define BUTTONRT 11
#define BUTTONSTART 12
#define BUTTONSELECT 13
#define BUTTONHOME 14
#define CBUTTONUP 15
#define CBUTTONDOWN 16
#define CBUTTONLEFT 17
#define CBUTTONRIGHT 18
#define BUTTONMODUP 19
#define BUTTONMODDOWN 20
#define BUTTONL3 21
#define BUTTONR3 22
#define DPADUP 23
#define DPADDOWN 24
#define DPADLEFT 25
#define DPADRIGHT 26

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

uint8_t fTwoIP(bool isLOW, bool isHIGH, bool& wasLOW, bool& wasHIGH);

bool wasLEFT = false;
bool wasRIGHT = false;
bool wasUP = false;
bool wasDOWN = false;

bool wasCLEFT = false;
bool wasCRIGHT = false;
bool wasCUP = false;
bool wasCDOWN = false;

bool lockLEFT = false;
bool lockRIGHT = false;
bool lockUP = false;
bool lockDOWN = false;

bool lockCLEFT = false;
bool lockCRIGHT = false;
bool lockCUP = false;
bool lockCDOWN = false;

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
  SetupHardware();
  GlobalInterruptEnable();
}

void loop() {
    buttonRead();
    processButtons();
    HID_Task();
    // We also need to run the main USB management task.
    USB_USBTask();
}

void buttonRead()
{
  if (joystickUP.update()) {buttonStatus[BUTTONUP] = joystickUP.fell();}
  if (joystickDOWN.update()) {buttonStatus[BUTTONDOWN] = joystickDOWN.fell();}
  if (joystickLEFT.update()) {buttonStatus[BUTTONLEFT] = joystickLEFT.fell();}
  if (joystickRIGHT.update()) {buttonStatus[BUTTONRIGHT] = joystickRIGHT.fell();}

  if (buttonModUp.update()) {buttonStatus[BUTTONMODUP] = buttonModUp.fell();}
  if (buttonModDown.update()) {buttonStatus[BUTTONMODDOWN] = buttonModDown.fell();}

  if (buttonStatus[BUTTONMODUP] && buttonStatus[BUTTONMODDOWN]) {
    buttonStatus[BUTTONB] = false;
    buttonStatus[BUTTONY] = false;
    buttonStatus[BUTTONRB] = false;
    buttonStatus[BUTTONRT] = false;
    buttonStatus[BUTTONLT] = false;

    buttonStatus[CBUTTONUP] = false;
    buttonStatus[CBUTTONDOWN] = false;
    buttonStatus[CBUTTONLEFT] = false;
    buttonStatus[CBUTTONRIGHT] = false;
    
    if (buttonB.update()) {buttonStatus[BUTTONSTART] = buttonB.fell();}  
    if (buttonY.update()) {buttonStatus[BUTTONHOME] = buttonY.fell();}
    
    if (buttonRB.update()) {buttonStatus[BUTTONSELECT] = buttonRB.fell();}
    if (buttonRT.update()) {buttonStatus[BUTTONR3] = buttonRT.fell();}
    if (buttonLT.update()) {buttonStatus[BUTTONL3] = buttonLT.fell();}

    if (cstickUP.update()) {buttonStatus[DPADUP] = cstickUP.fell();}
    if (cstickDOWN.update()) {buttonStatus[DPADDOWN] = cstickDOWN.fell();}
    if (cstickLEFT.update()) {buttonStatus[DPADLEFT] = cstickLEFT.fell();}
    if (cstickRIGHT.update()) {buttonStatus[DPADRIGHT] = cstickRIGHT.fell();}
  } else {
    if (cstickUP.update()) {buttonStatus[CBUTTONUP] = cstickUP.fell();}
    if (cstickDOWN.update()) {buttonStatus[CBUTTONDOWN] = cstickDOWN.fell();}
    if (cstickLEFT.update()) {buttonStatus[CBUTTONLEFT] = cstickLEFT.fell();}
    if (cstickRIGHT.update()) {buttonStatus[CBUTTONRIGHT] = cstickRIGHT.fell();}
    
    buttonStatus[BUTTONSTART] = false;
    buttonStatus[BUTTONHOME] = false;
    buttonStatus[BUTTONSELECT] = false;
    
    buttonStatus[BUTTONR3] = false;
    buttonStatus[BUTTONL3] = false;

    buttonStatus[DPADUP] = false;
    buttonStatus[DPADDOWN] = false;
    buttonStatus[DPADLEFT] = false;
    buttonStatus[DPADRIGHT] = false;
    
    if (buttonB.update()) {buttonStatus[BUTTONB] = buttonB.fell();}  
    if (buttonY.update()) {buttonStatus[BUTTONY] = buttonY.fell();}
    if (buttonRB.update()) {buttonStatus[BUTTONRB] = buttonRB.fell();}
    if (buttonStatus[BUTTONMODUP] || buttonStatus[BUTTONMODDOWN]) {
      buttonStatus[BUTTONX] = buttonY.fell();
    }
  }
  
  if (buttonA.update()) {buttonStatus[BUTTONA] = buttonA.fell();}
  if (buttonLT.update()) {buttonStatus[BUTTONLT] = buttonLT.fell();}
  if (buttonRT.update()) {buttonStatus[BUTTONRT] = buttonRT.fell();}
}

void processButtons() {
  bool isUP          = (buttonStatus[BUTTONUP]);
  bool isLEFT        = (buttonStatus[BUTTONLEFT]);
  bool isDOWN        = (buttonStatus[BUTTONDOWN]);
  bool isRIGHT       = (buttonStatus[BUTTONRIGHT]);
  
  bool isModUp       = (buttonStatus[BUTTONMODUP]);
  bool isModDown     = (buttonStatus[BUTTONMODDOWN]);

  bool isB           = (buttonStatus[BUTTONB]);
  bool isL           = (buttonStatus[BUTTONLT]);
  bool isR           = (buttonStatus[BUTTONRT]);
  
  bool isCDOWN       = (buttonStatus[CBUTTONDOWN]);
  bool isCRIGHT      = (buttonStatus[CBUTTONRIGHT]);
  bool isCLEFT       = (buttonStatus[CBUTTONLEFT]);
  bool isCUP         = (buttonStatus[CBUTTONUP]);

  uint8_t controlX = 128;
  uint8_t controlY = 128;
  uint8_t cstickX = 128;
  uint8_t cstickY = 128;

  int8_t positionX = 0;
  int8_t positionY = 0;
  int8_t positionCX = 0;
  int8_t positionCY = 0;

  bool HORIZONTAL = false;
  bool VERTICAL = false;
  bool DIAGONAL = false;

  /********* SOCD *********/
  controlX = fTwoIP(isLEFT, isRIGHT, wasLEFT, wasRIGHT);
  controlY = fTwoIP(isDOWN, isUP, wasDOWN, wasUP);
  cstickX = fTwoIP(isCLEFT, isCRIGHT, wasCLEFT, wasCRIGHT);
  cstickY = fTwoIP(isCDOWN, isCUP, wasCDOWN, wasCUP);

  if (controlX != 128) {
    HORIZONTAL = true;
    if (controlX == minStickValue) positionX = -1;
    else positionX = 1;
  }
  if (controlY != 128) {
    VERTICAL = true;
    if (controlY == minStickValue) positionY = -1;
    else positionY = 1;
  }
  if (HORIZONTAL && VERTICAL) DIAGONAL = true;

  if (cstickX != 128) {
    if (cstickX == minStickValue) positionCX = -1;
    else positionCX = 1;
  }
  if (cstickY != 128) {
    if (cstickY == minStickValue) positionCY = -1;
    else positionCY = 1;
  }
  /********* Modifiers *********/

  // states
  // movement - no buttons pressed, two different speeds
  // c stick - mod up, mod down
  // specials - mod up will go higher towards y, mod down lower towards x

  if (isModUp || isModDown) {   
    // horizontal tilts, 
    if (cstickX != 128) {
      cstickY = 128 + (isModUp ? 60 : -60);
    } else if (isL || isR) {
      if (HORIZONTAL) {
        controlX = 128 + (positionX * 51);
      }
      if (VERTICAL) {
        controlY = 128 + (positionY * 51);
      }
      if (DIAGONAL) {
        if (isModDown) {
          controlX = 128 + (positionX * 40);
          controlY = 128 + (positionY * 68);
        }
      }    
    } else if (DIAGONAL) {
      if (isModUp) {
        if (positionY < 0) {
          if (positionX < 0) {
            controlX = 255 * positionY;
            controlY = 64;  
          } else {
            controlX = 255 * positionX;
            controlY = (128 + 64) * positionY;   
          }
        } else {
          if (positionX < 0) {
            controlX = 64;
            controlY = 255 * positionY;  
          } else {
            controlX = (128 + 64) * positionX;
            controlY = 255 * positionY;   
          }
        }
      } else {
        if (positionY < 0) {
          if (positionX < 0) {
            controlX = 64;
            controlY = 255 * positionY;  
          } else {
            controlX = (128 + 64) * positionX;
            controlY = 255 * positionY;  
          }
        } else {
          if (positionX < 0) {
            controlX = 255 * positionX;
            controlY = 255 - 64;  
          } else {
            controlX = 255 * positionX;
            controlY = (128 + 64) * positionY;  
          }
        }
      }
    } else if (HORIZONTAL) {
      int8_t modifier = positionX * (isModDown ? 50 : 80);
      controlX = 128 + modifier;
    } else if (VERTICAL) {
      int8_t modifier = positionY * (isModDown ? 50 : 80);
      controlY = 128 + modifier;
    }
  } else {
    if (isLEFT && isRIGHT && !VERTICAL)
      controlX = 128 + (positionX * 100);  

    if (isL || isR) {
      if (HORIZONTAL) controlX = 128 + (positionX * 100);
      if (VERTICAL) controlY = 128 + (positionY * 100);
      if (HORIZONTAL && (positionY == -1)) {
        controlX = 128 + (positionX * 57);
        controlX = 128 + (positionX * 100);
        controlY = minStickValue;
      }
    }
  }
  
  if (buttonStatus[BUTTONA]) {ReportData.Button |= A_MASK_ON;}
  if (buttonStatus[BUTTONB]) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONX]) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONY]) {ReportData.Button |= Y_MASK_ON;}

  if (buttonStatus[BUTTONLB]) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB]) {ReportData.Button |= RB_MASK_ON;}

  if (buttonStatus[BUTTONLT]) {ReportData.Button |= ZL_MASK_ON;}
  if (buttonStatus[BUTTONRT]) {ReportData.Button |= ZR_MASK_ON;}

  if (buttonStatus[BUTTONSTART]){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONSELECT]){ReportData.Button |= SELECT_MASK_ON;}
  if (buttonStatus[BUTTONHOME]){ReportData.Button |= HOME_MASK_ON;}

  if (buttonStatus[BUTTONL3]){ReportData.Button |= L3_MASK_ON;}
  if (buttonStatus[BUTTONR3]){ReportData.Button |= R3_MASK_ON;}

  if (buttonStatus[DPADUP]){ReportData.HAT = DPAD_UP_MASK_ON;}
  else if (buttonStatus[DPADDOWN]){ReportData.HAT = DPAD_DOWN_MASK_ON;}
  else if (buttonStatus[DPADLEFT]){ReportData.HAT = DPAD_LEFT_MASK_ON;}
  else if (buttonStatus[DPADRIGHT]){ReportData.HAT = DPAD_RIGHT_MASK_ON;}
  else { ReportData.HAT = DPAD_NOTHING_MASK_ON; }
  
  // Control sticks
  
  ReportData.LX = controlX;//1 * (controlX - 128);
  ReportData.LY = (255-controlY);//-1 * (controlY - 128);
  ReportData.RX = cstickX;//1 * (cstickX - 128);
  ReportData.RY = (255-cstickY);//-1 * (cstickY - 128);
}

uint8_t fTwoIP(bool isLOW, bool isHIGH, bool& wasLOW, bool& wasHIGH) {
  uint8_t control = 128;
  if (isLOW && wasHIGH)
    control = minStickValue;
  if (isHIGH && wasLOW)
    control = maxStickValue;
  if (!isLOW && isHIGH) {
    control = maxStickValue;
    wasHIGH = true;
    wasLOW = false;
  }
  if (isLOW && !isHIGH) {
    control = minStickValue;
    wasLOW = true;
    wasHIGH = false;
  }
  return control;
}
