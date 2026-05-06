#include "Wire.h"

#define BUZZPIN 4
#define DEVMODE 1
#define I2CADDR 0x26

#define GPSFIXTONE 2048

#define LOOPDELAY 100 // Loop delay in ms

#define LAUNCHZTHRESH 745 // Approx 2.5g - max detect is 3g
#define LAUNCHTIMETHRESH 500 //Amount of time in ms above Z threshold before we decide we're launched
#define LAUNCHCOUNTTHRESH (int)(LAUNCHTIMETHRESH/LOOPDELAY) //Threshold Count = time threshold over loop delay time

#define XACCELPIN A6
#define YACCELPIN A7
#define ZACCELPIN A8

volatile bool launchDetect = false;
int launchZAboveThresh = 0;


//I2C variables
enum i2cProt {
  INVALID = 0,
  GPS,
  REQ
};

static char i2cProtText[3][4] = {
  "INV",
  "GPS",
  "REQ"
};

enum i2cReq {
  NOTREQ = 0,
  TEST,	//1 byte reply - 0xF
  XMIT_RDY	//1 byte reply - 0 = false, 1 = true
};

char i2cData[33]; //HW buffer of 32B, +1 for NULL since the protocol is string-based
enum i2cReq i2cReqType = NOTREQ;
volatile bool i2cNewData = false;
volatile bool requestTest = false;


bool gpsFixed = false;
bool gpsFirstFix = true;


void setup() {
  int zAxis = 0;
  int yAxis = 0;
  int xAxis = 0;

  Wire.begin(I2CADDR); //Start in I2C Slave mode.
  Wire.onReceive(receiveI2C);
  Wire.onRequest(requestI2C);

  pinMode(BUZZPIN, OUTPUT);
  xAxis = analogRead(XACCELPIN);
  yAxis = analogRead(YACCELPIN);
  zAxis = analogRead(ZACCELPIN);
  if (DEVMODE) {
    Serial.begin(9600);
    Serial.printf("Z: %i, Y: %i, X: %i\n", zAxis, yAxis, xAxis);
  }
  tone(BUZZPIN, xAxis, 200);
  delay(500);
  tone(BUZZPIN, yAxis, 200);
  delay(500);
  tone(BUZZPIN, zAxis, 200);
}

void updateLaunchDetect() {
  int zAxis = 0;
  zAxis = analogRead(ZACCELPIN);
  if (DEVMODE) {
    Serial.printf("Update zAxis: %i\n", zAxis);
  }
  if (zAxis >= LAUNCHZTHRESH) {
    if (++launchZAboveThresh >= LAUNCHCOUNTTHRESH) {
      launchDetect = true;
      if (DEVMODE) {
        Serial.printf("!!!LAUNCH DETECETED!!!\n");
        tone(BUZZPIN, 200, 500);
      }
    }
  } else {
    launchZAboveThresh = 0;
  }

}


void gpsFixBeep() {
  gpsFirstFix = false;
  tone(BUZZPIN, GPSFIXTONE, 200);
  delay(500);
  tone(BUZZPIN, GPSFIXTONE, 200);
  delay(500);
  tone(BUZZPIN, GPSFIXTONE, 200);
}

void testBeep() {
  requestTest = false;
  if (!DEVMODE) return;
  tone(BUZZPIN, GPSFIXTONE*2, 1000);
  delay(1000);
}

void receiveI2C(int count) {
  int i = 0;
  while (Wire.available() && i < 32) {
    i2cData[i++] = Wire.read();
  }
  i2cData[i] = '\0';
  if (DEVMODE) Serial.printf("I2C receive %s\n", i2cData);
  i2cNewData = true;
}

void requestI2C() {
  switch (i2cReqType) {
    case TEST: //Test request, only valid response is 0xF
      Wire.write(0xF);
      requestTest = true;
      break;
    case XMIT_RDY: //Asking if we're ready for transmission - once we've launched
      Wire.write((char)launchDetect);
      break;
    default:
      Wire.write(0xFF); //Invalid response
      if (DEVMODE) Serial.printf("I2C unhandled request %i\n", i2cReqType);
      break;
  }
}

int i2cProcessData() {
  char *token;
  enum i2cProt prot = INVALID;
  int payload = -1, ret = -1;

  if (i2cNewData) {
    if ((token = strtok(i2cData, ":")) != NULL) {
      if ((strstr(i2cProtText[GPS], token)) != NULL) {
        prot = GPS;
      } else if ((strstr(i2cProtText[REQ], token)) != NULL) {
        prot = REQ;
      } else {
        if (DEVMODE) Serial.printf("i2c Unrecognized protocol: '%s'\n", i2cData);
        ret = -1;
        goto out;
      }
    }

    token = strtok(NULL, ":");
    if (token != NULL) {
      payload = atoi(token);
    }

    ret = prot;
    switch (prot) {
      case GPS:
        gpsFixed = (bool)payload;
        if (gpsFirstFix && gpsFixed) gpsFixBeep();
        break;
      case REQ:
        i2cReqType = (enum i2cReq)payload;
        break;
      default:
        if (DEVMODE) Serial.printf("i2c Unhandled protocol: '%s'\n", i2cData);
        ret = -1;
        goto out;
    }
  }

out:

  i2cNewData = false;
  return ret;
}


void loop() {

  if (!launchDetect) updateLaunchDetect();
  if (i2cNewData) i2cProcessData();
  if (DEVMODE && requestTest) testBeep();

  delay(LOOPDELAY);
}
