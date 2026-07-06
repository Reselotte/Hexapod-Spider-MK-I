#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "motion.h"
#include "Arduino_RouterBridge.h"

// ================ PCA9685 =================
Adafruit_PWMServoDriver pwm1(0x40); // ขาซ้าย
Adafruit_PWMServoDriver pwm2(0x41); // ขาขวา

//============initial===================
int d = 100;
float currentAngle[6][3];

// ===== Motion Engine State =====
const int (*currentLUT)[6][3] = nullptr;
int currentLength = 0;
int currentFrame = 0;
unsigned long lastUpdate = 0;

int direction = 1;   // 1 = forward, -1 = reverse
int currentSpeed = 100;   // เปลี่ยนจาก motionSpeed
bool loopMotion = true;

// ================= SERVO PARAM =================
#define SERVO_FREQ 50

#define SERVOMIN 102
#define SERVOMID 307
#define SERVOMAX 512
#define DELAY_MS 20


///====offset====
static int left_offset_ticks[3][3] = {
  {-5,  10,   0},
  {-15,  5, -20},
  {20, -10,  10}
};

static int right_offset_ticks[3][3] = {
  {20, -10,   0},
  {-15,  0,  -5},
  {-10,  0, -20}
};

// ================= FUNCTION =================
uint16_t angleToPulse(int angle) {
  angle = constrain(angle, 0, 180);

  if (angle <= 90) {
    return map(angle, 0, 90, SERVOMIN, SERVOMID);
  } else {
    return map(angle, 90, 180, SERVOMID, SERVOMAX);
  }
}

//====global====
void setHexapodAngle(int leg, int joint, int angle);
void playMotionAngle(const int lut[][6][3], int length, int speedDelay);

// ================= SETUP =================
void setup() {
  Bridge.begin();   // begin
  Bridge.provide("motion_control", motion_control);
  
  Monitor.begin();
  Wire.begin();
  Wire.setClock(100000);

  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);

  pwm2.begin();
  pwm2.setPWMFreq(SERVO_FREQ);

  // playMotionAngle(lut_standby, lut_standby_length, 100);
}


// ================= LOOP =================
void loop() {
      if (currentLUT != nullptr) {

    if (millis() - lastUpdate >= currentSpeed) {
         lastUpdate = millis();
      for (int leg = 0; leg < 6; leg++) {
        for (int joint = 0; joint < 3; joint++) {
          setHexapodAngle(leg, joint, currentLUT[currentFrame][leg][joint]);
        }
      }

     currentFrame += direction;

if (currentFrame >= currentLength || currentFrame < 0) {

    if (loopMotion) {
        currentFrame = (direction == 1) ? 0 : currentLength - 1;
    } else {
        currentLUT = nullptr;  //
    }
  }
}}
  // playMotionAngle(lut_walk_1, lut_walk_1_length, 30);
  // playMotionAngle(lut_walk_1s, lut_walk_1s_length, 30);



  // playMotionAngle(lut_standby, lut_standby_length, 30);
  // delay(1000);
  // playMotionAngle(lut_sit, lut_sit_length, 30);
  // delay(1000);
  // playMotionAngle(walk_7_frame, walk_7_frame_length, 100);
  
  // playMotionAngle(lut_spin_1, lut_spin_1_length, 100);
  // playMotionAngle(lut_spin_2, lut_spin_2_length, 100);
  // playMotionAngle(lut_stand_90, lut_stand_90_length, 100);
  // playMotionAngle(lut_walk_3, lut_walk_3_length,100);
  // delay(108);
  // playMotionAngleReverse(lut_walk_3, lut_walk_3_length, 30);
  
  // playMotionAngle(lut_walk_101, lut_walk_101_length, 50);
  // delay(d);
  // playMotionAngle(lut_walk_102, lut_walk_102_length, 50);
  // delay(d);
  // playMotionAngle(lut_walk_103, lut_walk_103_length, 50);
  // delay(d);
  // playMotionAngle(lut_walk_104, lut_walk_104_length, 50);
  // delay(d);
  // playMotionAngle(lut_walk_105, lut_walk_105_length, 50);
  // playMotionAngle(walk, walk_length, 2);
   // playMotionAngle(walk_1, walk_1_length, 300);
  
  // playMotionAngle(lut_walk_2, lut_walk_2_length, 100);
  // playMotionAngle(lut_lean_2, lut_lean_2_length, 30);



// void motion_control(int cmd) {
//   if      (cmd == 1) playMotionAngle(lut_standby, lut_standby_length, 100);
//   else if (cmd == 2) playMotionAngle(sit, sit_length, 100);
    
//   else if (cmd == 3) playMotionAngle(walk_7_frame, walk_7_frame_length, 100);
//   else if (cmd == 4) playMotionAngleReverse(walk_7_frame, walk_7_frame_length, 100);

//   else if (cmd == 5) playMotionAngle(lut_spin_1, lut_spin_1_length, 100);
//   else if (cmd == 6) playMotionAngle(lut_spin_2, lut_spin_2_length, 100);  
}



// ================= SERVO CONTROL =================
void setHexapodAngle(int leg, int joint, int angle) {

  int pin = (leg % 3) * 4 + joint;
  uint16_t pulse = angleToPulse(angle);

  int offset = 0;

  if (leg < 3) {
    // ===== ขาซ้าย =====
    offset = left_offset_ticks[leg][joint];
    pulse = constrain(pulse + offset, SERVOMIN, SERVOMAX);
    pwm1.setPWM(pin, 0, pulse);
  } else {
    // ===== ขาขวา =====
    offset = right_offset_ticks[leg - 3][joint];
    pulse = constrain(pulse + offset, SERVOMIN, SERVOMAX);
    pwm2.setPWM(pin, 0, pulse);
  }
}

  // ================= SERVO CONTROL REVERSE =================
void playMotionAngleReverse(const int lut[][6][3], int length, int speedDelay) {
  for (int frame = length - 1; frame >= 0; frame--) {
    for (int leg = 0; leg < 6; leg++) {
      for (int joint = 0; joint < 3; joint++) {
        setHexapodAngle(leg, joint, lut[frame][leg][joint]);
      }
    }
    delay(speedDelay);
  }
}


// ================= MOTION ENGINE =================
void playMotionAngle(const int lut[][6][3], int length, int speedDelay) {
  for (int frame = 0; frame < length; frame++) {
    for (int leg = 0; leg < 6; leg++) {
      for (int joint = 0; joint < 3; joint++) {
        setHexapodAngle(leg, joint, lut[frame][leg][joint]);
      }
    }
    delay(speedDelay);
  }
}

//===================cmd=====================

void motion_control(int cmd) {
  Monitor.print("Received command: ");
  Monitor.println(cmd);
   if (cmd == 1) {   // stand
    currentLUT = lut_standby;
    currentLength = lut_standby_length;
    currentSpeed = 40;
    direction = 1;
    loopMotion = false;   // 🔥 เล่นรอบเดียว
  }

  else if (cmd == 2) {  // sit
    currentLUT = lut_sit;
    currentLength = lut_sit_length;
    currentSpeed = 40;
    direction = 1;
    loopMotion = false;   // 🔥 เล่นรอบเดียว
  }

  else if (cmd == 3) {  // walk
    currentLUT = lut_walk_3;
    currentLength = lut_walk_3_length;
    currentSpeed = 20;
    direction = 1;
    loopMotion = true;    // 🔥 วนต่อเนื่อง
  }

  else if (cmd == 4) {  // walk back
    currentLUT = lut_walk_3;
    currentLength = lut_walk_3_length;
    currentSpeed = 20;
    direction = -1;
    loopMotion = true;
  }

    else if (cmd == 5) {  // walk
    currentLUT = lut_spin_1;
    currentLength = lut_spin_1_length;
    currentSpeed = 150;
    direction = 1;
    loopMotion = true;    // 🔥 วนต่อเนื่อง
  }

   else if (cmd == 6) {  // walk
    currentLUT = lut_spin_2;
    currentLength = lut_spin_2_length;
    currentSpeed = 150;
    direction = 1;
    loopMotion = true;    // 🔥 วนต่อเนื่อง
  }

   else if (cmd == 7) {  // sit
    currentLUT = lut_stand_90;
    currentLength = lut_stand_90_length;
    currentSpeed = 40;
    direction = 1;
    loopMotion = false;   // 🔥 เล่นรอบเดียว
  }
  currentFrame = (direction == 1) ? 0 : currentLength - 1;
}
