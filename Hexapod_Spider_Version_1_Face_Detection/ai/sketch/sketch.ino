#include "Arduino_RouterBridge.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "motion.h"

int d = 100;
float currentAngle[6][3];

#define SERVO_FREQ 50
#define SERVOMIN   102
#define SERVOMID   307
#define SERVOMAX   512
#define DELAY_MS   10

const int (*currentLUT)[6][3] = nullptr;
int currentLength = 0;
int currentFrame  = 0;

unsigned long lastUpdate = 0;

int  direction    = 1;    // 1 = forward, -1 = reverse
int  currentSpeed = 100;  // เปลี่ยนจาก motionSpeed
bool loopMotion   = true;

// ================= PCA9685 =================
Adafruit_PWMServoDriver pwm1(0x40);  // ขาซ้าย
Adafruit_PWMServoDriver pwm2(0x41);  // ขาขวา


// ================= onZone =================
void onZone(int zone) {

  delay(200);

  Monitor.println(zone);
  Monitor.print("Received command: ");
  Monitor.println(zone);

  if (zone == 3) {                     // stand
    currentLUT    = lut_standby;
    currentLength = lut_standby_length;
    currentSpeed  = 40;
    direction     = 1;
    loopMotion    = false;
  }
  else if (zone == 1) {                // spin 1
    currentLUT    = lut_spin_1;
    currentLength = lut_spin_1_length;
    currentSpeed  = 60;
    direction     = 1;
    loopMotion    = true;
  }
  else if (zone == 2) {                // spin 2
    currentLUT    = lut_spin_2;
    currentLength = lut_spin_2_length;
    currentSpeed  = 60;
    direction     = 1;
    loopMotion    = true;
  }

  currentFrame = (direction == 1) ? 0 : currentLength - 1;
}


// ================= SETUP =================
void setup() {

  Monitor.begin();
  Bridge.begin();
  Bridge.provide("onZone", onZone);

  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);   // 100kHz (เสถียรกว่า)

  Serial.println("SETUP START");

  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);

  pwm2.begin();
  pwm2.setPWMFreq(SERVO_FREQ);

  for (int leg = 0; leg < 6; leg++) {
    for (int joint = 0; joint < 3; joint++) {
      currentAngle[leg][joint] = lut_standby[0][leg][joint];
    }
  }

  delay(500);
  Serial.println("Hexapod Ready (ANGLE LUT)");
}


// ================= FUNCTION =================
uint16_t angleToPulse(int angle) {

  angle = constrain(angle, 0, 180);

  if (angle <= 90) {
    return map(angle, 0, 90, SERVOMIN, SERVOMID);
  } else {
    return map(angle, 90, 180, SERVOMID, SERVOMAX);
  }
}

void setHexapodAngle(int leg, int joint, int angle);
void playMotionAngle(const int lut[][6][3], int length, int speedDelay);


// ===== SERVO OFFSET (ticks) =====
static int left_offset_ticks[3][3] = {
  { -5,  10,   0 },
  { -15,  5, -20 },
  {  20, -10,  10 }
};

static int right_offset_ticks[3][3] = {
  {  20, -10,   0 },
  { -15,   0,  -5 },
  { -10,   0, -20 }
};


// ================= LOOP =================
void loop() {

  if (currentLUT != nullptr) {

    if (millis() - lastUpdate >= currentSpeed) {

      lastUpdate = millis();

      for (int leg = 0; leg < 6; leg++) {
        for (int joint = 0; joint < 3; joint++) {
          setHexapodAngle(leg, joint,
                          currentLUT[currentFrame][leg][joint]);
        }
      }

      currentFrame += direction;

      if (currentFrame >= currentLength || currentFrame < 0) {

        if (loopMotion) {
          currentFrame = (direction == 1)
                           ? 0
                           : currentLength - 1;
        } else {
          currentLUT = nullptr;
        }
      }
    }
  }
}


// ================= SERVO CONTROL =================
void setHexapodAngle(int leg, int joint, int angle) {

  int pin = (leg % 3) * 4 + joint;
  uint16_t pulse = angleToPulse(angle);
  int offset = 0;

  if (leg < 3) {
    offset = left_offset_ticks[leg][joint];
    pulse  = constrain(pulse + offset, SERVOMIN, SERVOMAX);
    pwm1.setPWM(pin, 0, pulse);
  }
  else {
    offset = right_offset_ticks[leg - 3][joint];
    pulse  = constrain(pulse + offset, SERVOMIN, SERVOMAX);
    pwm2.setPWM(pin, 0, pulse);
  }
}


// ================= MOTION ENGINE =================
void playMotionAngle(const int lut[][6][3], int length, int speedDelay) {

  for (int frame = 0; frame < length; frame++) {

    for (int leg = 0; leg < 6; leg++) {
      for (int joint = 0; joint < 3; joint++) {
        setHexapodAngle(leg, joint,
                       lut[frame][leg][joint]);
      }
    }

    delay(speedDelay);
  }
}


void playMotionAngleReverse(const int lut[][6][3], int length, int speedDelay) {

  for (int frame = length - 1; frame >= 0; frame--) {

    for (int leg = 0; leg < 6; leg++) {
      for (int joint = 0; joint < 3; joint++) {
        setHexapodAngle(leg, joint,
                       lut[frame][leg][joint]);
      }
    }

    delay(speedDelay);
  }
}
