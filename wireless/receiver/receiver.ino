#include "LUFAConfig.h"
#include <LUFA.h>
#include "Joystick.h"
#define pinOBLED 21  //Onboard LED pin
#include <SPI.h>             /* to handle the communication interface with the modem*/
#include <nRF24L01.h>        /* to handle this particular modem driver*/
#include <RF24.h>            /* the library which helps us to control the radio modem*/

RF24 radio(3, 4);  // CE, CSN
const uint64_t address = 0xF0F0F0F0E1LL;

const uint8_t minStickValue = 0;
const uint8_t maxStickValue = 255;

byte buttonStatus[29];

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
#define BUTTONX 18
#define BUTTONLB 19
#define BUTTONSTART 20
#define BUTTONSELECT 21
#define BUTTONHOME 22
#define BUTTONL3 23
#define BUTTONR3 24
#define DPADUP 25
#define DPADDOWN 26
#define DPADLEFT 27
#define DPADRIGHT 28

uint8_t fTwoIP(bool isLOW, bool isHIGH, bool& wasLOW, bool& wasHIGH);

bool wasLEFT = false;
bool wasRIGHT = false;
bool wasUP = false;
bool wasDOWN = false;

bool wasCLEFT = false;
bool wasCRIGHT = false;
bool wasCUP = false;
bool wasCDOWN = false;

void setupPins(){        
  pinMode(pinOBLED, OUTPUT);  
  //Set the LED to low to make sure it is off
  digitalWrite(pinOBLED, HIGH);
}

void setup() {
  
  // put your setup code here, to run once:
  radio.begin();                   /* Activate the modem*/
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  radio.powerUp();
  radio.openReadingPipe(0, address); /* Sets the address of receiver from which program will receive the data*/
  radio.startListening();          /*Setting modem in Receiver mode*/
  setupPins();
  SetupHardware();
  GlobalInterruptEnable();
//Serial.begin(9600);
}

void loop() {
  if (radio.available()) {
    byte rx_data[18] = { 0 };
    /* Variable to store received data */
    while (radio.available()) {
      radio.read(rx_data, 18);/* Read the received data and store in ' rx_data ' */
    }
    buttonRead(rx_data);
    processButtons();
  } else {
    processButtons();
  }
  HID_Task();
  // We also need to run the main USB management task.
  USB_USBTask();
}

void buttonRead(byte rx_data[18]) {
  buttonStatus[BUTTONUP] = rx_data[BUTTONUP] == 1;
  
  buttonStatus[BUTTONDOWN] = rx_data[BUTTONDOWN] == 1;
  buttonStatus[BUTTONLEFT] = rx_data[BUTTONLEFT] == 1;
  buttonStatus[BUTTONRIGHT] = rx_data[BUTTONRIGHT] == 1;

  buttonStatus[BUTTONMODUP] = rx_data[BUTTONMODUP] == 1;
  buttonStatus[BUTTONMODDOWN] = rx_data[BUTTONMODDOWN] == 1;

  if (buttonStatus[BUTTONMODUP] == 1 && buttonStatus[BUTTONMODDOWN] == 1) {
    buttonStatus[BUTTONB] = false;
    buttonStatus[BUTTONY] = false;
    buttonStatus[BUTTONRB] = false;
    buttonStatus[BUTTONRT] = false;
    buttonStatus[BUTTONLT] = false;

    buttonStatus[CBUTTONUP] = false;
    buttonStatus[CBUTTONDOWN] = false;
    buttonStatus[CBUTTONLEFT] = false;
    buttonStatus[CBUTTONRIGHT] = false;

    buttonStatus[BUTTONSTART] = rx_data[BUTTONB] == 1;
    buttonStatus[BUTTONHOME] = rx_data[BUTTONY] == 1;

    buttonStatus[BUTTONSELECT] = rx_data[BUTTONRB] == 1;
    buttonStatus[BUTTONR3] = rx_data[BUTTONRT] == 1;
    buttonStatus[BUTTONL3] = rx_data[BUTTONLT] == 1;

    buttonStatus[DPADUP] = rx_data[CBUTTONUP] == 1;
    buttonStatus[DPADDOWN] = rx_data[CBUTTONDOWN] == 1;
    buttonStatus[DPADLEFT] = rx_data[CBUTTONLEFT] == 1;
    buttonStatus[DPADRIGHT] = rx_data[CBUTTONRIGHT] == 1;
  } else {
    buttonStatus[CBUTTONUP] = rx_data[CBUTTONUP] == 1;
    buttonStatus[CBUTTONDOWN] = rx_data[CBUTTONDOWN] == 1;
    buttonStatus[CBUTTONLEFT] = rx_data[CBUTTONLEFT] == 1;
    buttonStatus[CBUTTONRIGHT] = rx_data[CBUTTONRIGHT] == 1;

    buttonStatus[BUTTONLT] = rx_data[BUTTONLT] == 1;
    buttonStatus[BUTTONRT] = rx_data[BUTTONRT] == 1;

    buttonStatus[BUTTONSTART] = false;
    buttonStatus[BUTTONHOME] = false;
    buttonStatus[BUTTONSELECT] = false;
    
    buttonStatus[BUTTONR3] = false;
    buttonStatus[BUTTONL3] = false;

    buttonStatus[DPADUP] = false;
    buttonStatus[DPADDOWN] = false;
    buttonStatus[DPADLEFT] = false;
    buttonStatus[DPADRIGHT] = false;

    buttonStatus[BUTTONB] = rx_data[BUTTONB] == 1;
    buttonStatus[BUTTONY] = rx_data[BUTTONY] == 1;
    buttonStatus[BUTTONRB] = rx_data[BUTTONRB] == 1;
    
    if (buttonStatus[BUTTONMODUP] == 1 || buttonStatus[BUTTONMODDOWN] == 1) {
      buttonStatus[BUTTONX] = rx_data[BUTTONY] == 1;
    }
  }

  buttonStatus[BUTTONA] = rx_data[BUTTONA] == 1;
}

void processButtons() {
  bool isUP          = (buttonStatus[BUTTONUP] == 1);
  bool isLEFT        = (buttonStatus[BUTTONLEFT] == 1);
  bool isDOWN        = (buttonStatus[BUTTONDOWN] == 1);
  bool isRIGHT       = (buttonStatus[BUTTONRIGHT] == 1);
  
  bool isModUp       = (buttonStatus[BUTTONMODUP] == 1);
  bool isModDown     = (buttonStatus[BUTTONMODDOWN] == 1);

  bool isB           = (buttonStatus[BUTTONB] == 1);
  bool isL           = (buttonStatus[BUTTONLT] == 1);
  bool isR           = (buttonStatus[BUTTONRT] == 1);
  
  bool isCDOWN       = (buttonStatus[CBUTTONDOWN] == 1);
  bool isCRIGHT      = (buttonStatus[CBUTTONRIGHT] == 1);
  bool isCLEFT       = (buttonStatus[CBUTTONLEFT] == 1);
  bool isCUP         = (buttonStatus[CBUTTONUP] == 1);

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
  
  if (buttonStatus[BUTTONB] == 1) {ReportData.Button |= B_MASK_ON;}
  if (buttonStatus[BUTTONX] == 1) {ReportData.Button |= X_MASK_ON;}
  if (buttonStatus[BUTTONY] == 1) {ReportData.Button |= Y_MASK_ON;}

  if (buttonStatus[BUTTONLB] == 1) {ReportData.Button |= LB_MASK_ON;}
  if (buttonStatus[BUTTONRB] == 1) {ReportData.Button |= RB_MASK_ON;}

  if (buttonStatus[BUTTONLT] == 1) {ReportData.Button |= ZL_MASK_ON;}
  if (buttonStatus[BUTTONRT] == 1) {ReportData.Button |= ZR_MASK_ON;}

  if (buttonStatus[BUTTONSTART] == 1){ReportData.Button |= START_MASK_ON;}
  if (buttonStatus[BUTTONSELECT] == 1){ReportData.Button |= SELECT_MASK_ON;}
  if (buttonStatus[BUTTONHOME] == 1){ReportData.Button |= HOME_MASK_ON;}

  if (buttonStatus[BUTTONL3] == 1){ReportData.Button |= L3_MASK_ON;}
  if (buttonStatus[BUTTONR3] == 1){ReportData.Button |= R3_MASK_ON;}

  if (buttonStatus[DPADUP] == 1){ReportData.HAT = DPAD_UP_MASK_ON;}
  else if (buttonStatus[DPADDOWN] == 1){ReportData.HAT = DPAD_DOWN_MASK_ON;}
  else if (buttonStatus[DPADLEFT] == 1){ReportData.HAT = DPAD_LEFT_MASK_ON;}
  else if (buttonStatus[DPADRIGHT] == 1){ReportData.HAT = DPAD_RIGHT_MASK_ON;}
  else { ReportData.HAT = DPAD_NOTHING_MASK_ON; }
  
  // Control sticks
  if (!(isCDOWN || isCUP || isCLEFT || isCRIGHT)) {
    if (buttonStatus[BUTTONA] == 1) {ReportData.Button |= A_MASK_ON;}
    ReportData.LX = controlX;//1 * (controlX - 128);  
    ReportData.LY = (255-controlY);//-1 * (controlY - 128);
  } else {
    int removeValueX = 31;
    int removeValueY = 41;
    ReportData.LX = max(min(cstickX, 255-removeValueX), removeValueX);
    ReportData.LY = min(max(255-cstickY, removeValueY), 255-removeValueY);
    ReportData.Button |= A_MASK_ON;
  }
  ReportData.RX = 128;
  ReportData.RY = 128;
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
