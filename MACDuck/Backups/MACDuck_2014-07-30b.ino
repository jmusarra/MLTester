#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"




Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // default i2c address - can be different if stacking

Adafruit_StepperMotor *motorShield1 = AFMS.getStepper(200, 1);   // Dimmer flag 1, 200 steps, connected to motorshield port 1
Adafruit_StepperMotor *motorShield2 = AFMS.getStepper(200,2);    // Dimmer flag 2, 200 steps, connected to motorshield port 2

void forwardstep1() {  
  motorShield1->onestep(FORWARD, MICROSTEP);    // steps can be SINGLE, DOUBLE, MICROSTEP, or MICROSTEP
}
void backwardstep1() {  
  motorShield1->onestep(BACKWARD, MICROSTEP);
}

void forwardstep2() {  
  motorShield2->onestep(FORWARD, MICROSTEP);    // change to MICROSTEP for dimmer flags
}
void backwardstep2() {  
  motorShield2->onestep(BACKWARD, MICROSTEP);
}

const int flagSteps = 355;   // 1340 quarter steps, 335 full steps
const byte dimmerSteps = 144;         // 18 full steps, 144 eighth-steps 
const byte colorWheelSteps = 177;     // 178 or 177? Feels like there's some inaccuracy.  
unsigned long now = 0;
byte slp = 12;
int testCount = 0;



// Motorshield motors, using functions to step:
AccelStepper dimmer1(forwardstep1, backwardstep1);    // Motorshield port 1 - dimmer flag 1. 18 full steps travel. - molex pins 1+3 & 2+4
AccelStepper dimmer2(forwardstep2, backwardstep2);    // Motorshield port 2 - dimmer flag 2. 18 full steps travel. - molex pins 5+6 & 7+8
// Pololu motors - 1-pin mode, step pin, direction pin:
AccelStepper cyan(1, 2, 3);          // cyan flags - molex pins 9+10 & 11+12
AccelStepper yellow(1, 4, 5);        // yellow flags - molex pins 13+14 & 15+16
AccelStepper magenta(1, 6, 7);       // magenta flags - molex pins 17+18 and 19+20
AccelStepper cto(1, 8, 9);           // Wash module CTO flags
AccelStepper colorWheel(1, 10, 11);  // Wash module color wheel

void setup() {
  Serial.begin(9600);
  AFMS.begin();
  pinMode(slp, OUTPUT);
  digitalWrite(slp, LOW);
  
  dimmer1.setMaxSpeed(500);
  dimmer1.setAcceleration(800);
  dimmer2.setMaxSpeed(500);
  dimmer2.setAcceleration(800);
  cyan.setMaxSpeed(500);
  cyan.setAcceleration(800);
  magenta.setMaxSpeed(500);
  magenta.setAcceleration(800);
  yellow.setMaxSpeed(500);
  yellow.setAcceleration(800);
  cto.setMaxSpeed(500);
  cto.setAcceleration(500);
  motorShield1->release();
  motorShield2->release();
  Serial.println("MacDuck Initialized...");
  now = millis();
 // init(dimmer1, dimmer2, cyan, yellow, magenta);
}


//void init() {

// run from setup() - move all connected motors sensorwise
// when sensor hit, mark that position as zero
// sleep

//}


void testDimmer(int reps) {                   // Mac dimmer blades should always be on the motorshield.
    dimmer1.move(dimmerSteps);
    dimmer2.move(dimmerSteps);
    dimmer1.run();
    dimmer2.run();
    delay(600);
    dimmer1.move(-dimmerSteps);
    dimmer2.move(-dimmerSteps);
    dimmer1.run();
    dimmer2.run();
}

void testColor(AccelStepper flag, int reps) {   
  int times = 0;
  digitalWrite(slp, HIGH);
    while (times < reps) {
      flag.move(355);
      flag.runToPosition();
      delay(600);
      flag.move(-355);
      flag.runToPosition();
      times++;
   }
   digitalWrite(slp, LOW);
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
  switch (inChar) {
    case 'x':
    Serial.println("HALT!");
       motorShield1->release();
       motorShield2->release();
       digitalWrite(slp, LOW);
       break;

    case 'd':
      Serial.println("Running dimmer check...");
      testDimmer(1);
      Serial.println("Complete.");
      break;

    case 'c':
      Serial.println("Running cyan check...");
      testColor(cyan, 1);
      Serial.println("Complete.");
      break;

    case 'y':
      Serial.println("Running yellow check...");
      testColor(yellow, 1);
      Serial.println("Complete.");
      break;

    case 'm':
      Serial.println("Running magenta check...");
      testColor(magenta, 1);
      Serial.println("Complete.");
      break;

    case 'o':
      Serial.println("Running CTO check...");
      testColor(cto, 1);
      Serial.println("Complete.");
      break;

    case 'w':
      Serial.println("Running color wheel check...");
      break;
    }
}
}

