#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f,20,4);  // set the LCD address to 0x3F, with 16 chars and 4 lines

Adafruit_MotorShield AFMS = Adafruit_MotorShield();              // default i2c address - change for additional stacked shields

Adafruit_StepperMotor *motorShield1 = AFMS.getStepper(200,1);    // Dimmer flag 1, 200 steps, connected to motorshield port 1
Adafruit_StepperMotor *motorShield2 = AFMS.getStepper(200,2);    // Dimmer flag 2, 200 steps, connected to motorshield port 2

void forwardstep1() {  
  motorShield1->onestep(FORWARD, MICROSTEP);    // Since dimmer flags, make microsteppy
}
void backwardstep1() {  
  motorShield1->onestep(BACKWARD, MICROSTEP);
}

void forwardstep2() {  
  motorShield2->onestep(FORWARD, MICROSTEP);    // Since dimmer flags, make microsteppy
}
void backwardstep2() {  
  motorShield2->onestep(BACKWARD, MICROSTEP);
}

const int flagSteps = 330;           // 1340 quarter steps, 335 full steps
const int dimmerSteps = 158;         // 18 full steps, 144 eighth-steps 
const byte colorWheelSteps = 177;    // 178 or 177? Feels like there's some inaccuracy.
byte slp = 12;
int cyanSwitch = 14;
int yelSwitch = 15;
int magSwitch = 16;
int buttonCyan = 17;
bool modInit = false;

// Motorshield motors, using functions to step:
AccelStepper dimmer1(forwardstep1, backwardstep1);    // Motorshield port 1 - dimmer flag 1. 18 full steps travel. - molex pins 1+3 & 2+4
AccelStepper dimmer2(forwardstep2, backwardstep2);    // Motorshield port 2 - dimmer flag 2. 18 full steps travel. - molex pins 5+7 & 6+8
// Pololu motors - 1-pin mode, step pin, direction pin:
AccelStepper cyan(1, 2, 3);          // cyan flags - molex pins 9+11 & 10+12
AccelStepper magenta(1, 4, 5);       // magenta flags - molex pins 17+19 and 18+20
                                     // magenta motor on profile module needs to be inverted
AccelStepper yellow(1, 6, 7);        // yellow flags - molex pins 13+15 & 14+16                                     
AccelStepper cto(1, 8, 9);           // Wash module CTO flags
                                     // cto motor also needs to be inverted?
AccelStepper colorWheel(1, 10, 11);  // Wash module color wheel

void setup() {
  for (int i=2; i<14; i++) {     // Make digital pins 2-13 be outputs
    pinMode(i, OUTPUT);
  }
  for (int j=14; j<18; j++) {    // Make analog pins 0-3 be inputs
    pinMode(j, INPUT);
}
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  AFMS.begin();
  
  dimmer1.setMaxSpeed(500);
  dimmer1.setAcceleration(800);
  dimmer2.setMaxSpeed(500);
  dimmer2.setAcceleration(800);
  cyan.setAcceleration(2000);
  cyan.setPinsInverted(true, false, false);
  magenta.setAcceleration(2000);
  magenta.setPinsInverted(true, false, false);
  yellow.setAcceleration(2000);
  cto.setAcceleration(500);
  //motorShield1->release();
  //motorShield2->release();
  digitalWrite(slp, LOW);
 

  lcd.print("MacDuck started.");

  if (modInit == false) {
    initModule();
  }
}


/*
run from setup() - move all connected motors sensorwise
when sensor hit, mark that position as zero
sleep
*/

void initModule() {
  delay(1500);
  lcd.clear();
  lcd.print("Calibrating module...");
  
  dimmer1.moveTo(-dimmerSteps);
  dimmer2.moveTo(-dimmerSteps);
  while (dimmer1.currentPosition() > -dimmerSteps) {
  dimmer1.run();
   }
  dimmer1.stop();
  while (dimmer2.currentPosition() > -dimmerSteps) {
  dimmer2.run();
   }
  dimmer2.stop();
  digitalWrite(slp, HIGH);
  
  cyan.setMaxSpeed(2000);
  yellow.setMaxSpeed(2000);
  magenta.setMaxSpeed(2000);
  cyan.moveTo(-flagSteps);
  yellow.moveTo(-flagSteps);
  magenta.moveTo(-flagSteps);

  while (digitalRead(cyanSwitch)==LOW) {
    cyan.run();
  }
  cyan.stop();
  cyan.runToPosition();
  
  while (digitalRead(yelSwitch)==LOW) {
    yellow.run();
  }
  yellow.stop();
  yellow.runToPosition();

  while (digitalRead(magSwitch)==LOW) {
    magenta.run();
  }
  magenta.stop();
  magenta.runToPosition();
  

motorShield1->release();
motorShield2->release();
digitalWrite(slp, LOW);
modInit = true;
Serial.println("Module calibrated.");
}



void testDimmer(int reps) {                   // Mac dimmer blades should always be on the motorshield.
  int dimmerTimes = 0; 
  while (dimmerTimes < reps) { 
    dimmer1.move(dimmerSteps);
    dimmer2.move(dimmerSteps);
    dimmer1.runToPosition();
    dimmer2.runToPosition();
    delay(600);
    dimmer1.move(-dimmerSteps);
    dimmer2.move(-dimmerSteps);
    dimmer1.runToPosition();
    dimmer2.runToPosition();
    dimmerTimes++;
    delay(1000);
  }
   motorShield1->release();
   motorShield2->release();

}

void testColor(AccelStepper flag, int reps=1, int spd=2000) {
  flag.setMaxSpeed(spd);
  int colorTimes = 0;
  digitalWrite(slp, HIGH);
  delay(10);
    while (colorTimes < reps) {
      flag.moveTo(310);
      while (flag.currentPosition() < 310) {
      flag.run();
      }
      delay(600);
      flag.moveTo(0);
      while (flag.currentPosition() > 0) {
      flag.run();
      }
      colorTimes++;
      delay(600);
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
      testColor(cyan, 3);
      Serial.println("Complete.");
      break;

    case 'y':
      Serial.println("Running yellow check...");
      testColor(yellow, 3);
      Serial.println("Complete.");
      break;

    case 'm':
      Serial.println("Running magenta check...");
      testColor(magenta, 3);
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
if (digitalRead(buttonCyan)==HIGH) {
   Serial.println("Running all flag check...");
   testColor(cyan, 1);
   testColor(yellow, 1);
   testColor(magenta, 1);
   Serial.println("Complete.");
}
}

