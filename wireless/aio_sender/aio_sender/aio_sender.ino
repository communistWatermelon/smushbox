#include <SPI.h>                  /* to handle the communication interface with the modem*/
#include <nRF24L01.h>             /* to handle this particular modem driver*/
#include <RF24.h>                 /* the library which helps us to control the radio modem*/
#include <printf.h>

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

#define BUTTONUP_PIN 7
#define BUTTONDOWN_PIN 8
#define BUTTONLEFT_PIN 6
#define BUTTONRIGHT_PIN 5
#define BUTTONA_PIN A1
#define BUTTONB_PIN A2
#define BUTTONY_PIN A3
#define BUTTONRB_PIN A4
#define BUTTONLT_PIN A5
#define BUTTONRT_PIN 1
#define CBUTTONUP_PIN 4
#define CBUTTONDOWN_PIN 3
#define CBUTTONLEFT_PIN 2
#define CBUTTONRIGHT_PIN A0
#define BUTTONMODUP_PIN A7
#define BUTTONMODDOWN_PIN 0

// can't use 9, 10, 11, 12, 13
void setupPins(){
  pinMode(BUTTONUP_PIN, INPUT_PULLUP);
  pinMode(BUTTONDOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTONLEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTONRIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTONA_PIN, INPUT_PULLUP);
  pinMode(BUTTONB_PIN, INPUT_PULLUP);
  pinMode(BUTTONY_PIN, INPUT_PULLUP);
  pinMode(BUTTONRB_PIN, INPUT_PULLUP);
  pinMode(BUTTONLT_PIN, INPUT_PULLUP);
  pinMode(BUTTONRT_PIN, INPUT_PULLUP);
  pinMode(CBUTTONUP_PIN, INPUT_PULLUP);
  pinMode(CBUTTONDOWN_PIN, INPUT_PULLUP);
  pinMode(CBUTTONLEFT_PIN, INPUT_PULLUP);
  pinMode(CBUTTONRIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTONMODUP_PIN, INPUT_PULLUP);
  pinMode(BUTTONMODDOWN_PIN, INPUT_PULLUP);
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
    delay(1); // need this delay for the read to work, thanks arduino
  }
  return total == LOW ? 1 : 0;
}

void buttonRead()
{
  buttonStatus[BUTTONUP] = digitalRead(BUTTONUP_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONDOWN] = digitalRead(BUTTONDOWN_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONLEFT] = digitalRead(BUTTONLEFT_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONRIGHT] = digitalRead(BUTTONRIGHT_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONMODUP] = readAnalogValue(BUTTONMODUP_PIN);
  buttonStatus[BUTTONMODDOWN] = digitalRead(BUTTONMODDOWN_PIN) == LOW ? 1 : 0;
  buttonStatus[CBUTTONUP] = digitalRead(CBUTTONUP_PIN) == LOW ? 1 : 0;
  buttonStatus[CBUTTONDOWN] = digitalRead(CBUTTONDOWN_PIN) == LOW ? 1 : 0;
  buttonStatus[CBUTTONLEFT] = digitalRead(CBUTTONLEFT_PIN) == LOW ? 1 : 0;
  buttonStatus[CBUTTONRIGHT] = digitalRead(CBUTTONRIGHT_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONB] = digitalRead(BUTTONB_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONY] = digitalRead(BUTTONY_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONRB] = digitalRead(BUTTONRB_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONA] = digitalRead(BUTTONA_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONLT] = digitalRead(BUTTONLT_PIN) == LOW ? 1 : 0;
  buttonStatus[BUTTONRT] = digitalRead(BUTTONRT_PIN) == LOW ? 1 : 0;
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
    previousSendTime = currentTime;
  }
}
