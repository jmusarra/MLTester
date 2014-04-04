/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

const int buttonPin = 2;     // the pin the button is connected to

int buttonState = 0;         // sets the initial state of the button to 'off'.



// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  myMotor->setSpeed(60);  // 10 rpm   
  myMotor->release();  //de-energizes the coils, so the motor won't heat up if it's just sitting there
  
   // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

//Ideally, do this in response to a button press:
void loop() {
  
  buttonState = digitalRead(buttonPin);
  
  if (buttonState == HIGH) {
  // Serial.println("Single coil steps");
  Serial.println("You pressed the button, let's spin the duck!");
  myMotor->step(50, FORWARD, SINGLE); 
  delay(50);
  myMotor->step(50, BACKWARD, SINGLE);
  delay(50);
  myMotor->step(50, FORWARD, SINGLE);
  delay(50);
  myMotor->step(50, BACKWARD, SINGLE);
  delay(50);
  myMotor->step(50, FORWARD, SINGLE);
  delay(50);
  myMotor->step(50, BACKWARD, SINGLE);
  delay(50);
  myMotor->step(50, FORWARD, SINGLE);
  delay(50);
  myMotor->step(50, BACKWARD, SINGLE);
  delay(50);
  myMotor->step(50, FORWARD, SINGLE);
  Serial.println("resting...");
  myMotor->release();
  
  }
  
else {
  myMotor->release();
}
//Then another button to make this happen:
//  Serial.println("1 full rev backward...");
//  myMotor->step(200, BACKWARD, MICROSTEP);
//  Serial.println("resting...");
//  myMotor->release();
//  delay(2000);
//And then just loop the whole thing, displaying the serial output on an LCD for the user.

 
 //Can probably do away with these steps? Research whether we need double/interleave/microstepping
 
 
 // Serial.println("Double coil steps");
 // myMotor->step(100, FORWARD, DOUBLE); 
 // myMotor->step(100, BACKWARD, DOUBLE);
  
 // Serial.println("Interleave coil steps");
 // myMotor->step(100, FORWARD, INTERLEAVE); 
 // myMotor->step(100, BACKWARD, INTERLEAVE); 
  
 // Serial.println("Microstep steps");
 // myMotor->step(50, FORWARD, MICROSTEP); 
 // myMotor->step(50, BACKWARD, MICROSTEP);
}
