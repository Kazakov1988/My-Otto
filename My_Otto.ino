//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-- Otto DIY invests time and resources providing open source code and hardware,  please support by purchasing kits from (https://www.ottodiy.com)
//-------------------------------------------------------------------------
//-- If you wish to use this software under Open Source Licensing, you must contribute all your source code to the community and all text above must be included in any redistribution
//-- in accordance with the GPL Version 2 when your application is distributed. See http://www.gnu.org/copyleft/gpl.html
//-------------------------------------------------------------------------
//-- The firmware is based on V11. 
//-- Suitable for the Otto Plus kit (the circuit must have a Bluetooth module, a touch button and an ultrasonic sensor HC-SR04). 
//-- The firmware has removed the LED matrix control and other elements that I do not use in my Otto, so it now takes only 68% of the Arduino memory.
//-- Otto knows how to respond to your stroking (touching the upper part of the body), and live for a some time satisfied without showing your love. 
//-- When the time has passed, as soon as someone appears within sight of the distance sensor, Otto will begin gesturing to ask to stroke it again. 
//-- At the same time, the robot retains full Bluetooth control using the official application. 
//-- Check out the comments in the firmware to change Otto's behavior and reaction as you see fit.
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <Oscillator.h>
#include <SerialCommand.h>
SoftwareSerial BTserial = SoftwareSerial(11, 12);
SerialCommand SCmd(BTserial);
#include <Otto9.h>
Otto9 Otto;

#define PIN_YL 2 // left leg, servo[0]
#define PIN_YR 3 // right leg, servo[1]
#define PIN_RL 4 // left foot, servo[2]
#define PIN_RR 5 // right foot, servo[3]
#define PIN_Trigger  8
#define PIN_Echo     9
#define PIN_Buzzer  13
#define PIN_NoiseSensor A6
#define PIN_Button   A0

int T = 1000;
int moveId = 0;
int moveSize = 15;
volatile bool buttonPushed = false;
int i = 0;

unsigned long timing;
unsigned long timeWithoutLove;               // Variable for counting time without stroking

void setup() {
  Serial.begin(9600);
  BTserial.begin(9600);

  Otto.init(PIN_YL, PIN_YR, PIN_RL, PIN_RR, true, PIN_NoiseSensor, PIN_Buzzer, PIN_Trigger, PIN_Echo);
  Otto.matrixIntensity(7);
  randomSeed(analogRead(PIN_NoiseSensor));
  pinMode(PIN_Button, INPUT);

  SCmd.addCommand("M", receiveMovement);
  SCmd.addCommand("H", receiveGesture);
  SCmd.addCommand("K", receiveSing);

  Otto.sing(S_connection);
  Otto.home();
  Otto.sing(S_happy);
  delay(200);
}

void loop() {
  SCmd.readSerial();
  if (Otto.getRestState() == false) {
    move(moveId);
  }

  if (millis() - timeWithoutLove > 300000) {  // Time that Otto can be without stroking (in ms)
    objectDetection ();
  }
  toTouch ();
}

void toTouch () {
  if (digitalRead(PIN_Button)) {              
    Otto.playGesture(OttoLove);               // What will Otto do if you stroke him
    timeWithoutLove = millis();
  }
}

void objectDetection () {
  if ((Otto.getDistance() < 90 )) {           // If an object appears at this distance, Otto will ask you to stroke it (in mm), maximum - 1500
    if (millis() - timing > 5000) {           // Frequency of retrying a request from Otto
      Otto.playGesture(OttoHappy);            // How Otto will ask to be stroked
      timing = millis();
    }
  }
}

void receiveMovement() {
  if (Otto.getRestState() == true) Otto.setRestState(false);
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) moveId = atoi(arg);
  else {
    moveId = 0;
  }
  arg = SCmd.next();
  if (arg != NULL) T = atoi(arg);
  else T = 1000;
  arg = SCmd.next();
  if (arg != NULL) moveSize = atoi(arg);
  else moveSize = 15;
}

void move(int moveId)
{
  bool manualMode = false;
  switch (moveId) {
    case 0: Otto.home();
      break;
    case 1: Otto.walk(1, T, 1);
      break;
    case 2: Otto.walk(1, T, -1);
      break;
    case 3: Otto.turn(1, T, 1);
      break;
    case 4: Otto.turn(1, T, -1);
      break;
    case 5: Otto.updown(1, T, moveSize);
      break;
    case 6: Otto.moonwalker(1, T, moveSize, 1);
      break;
    case 7: Otto.moonwalker(1, T, moveSize, -1);
      break;
    case 8: Otto.swing(1, T, moveSize);
      break;
    case 9: Otto.crusaito(1, T, moveSize, 1);
      break;
    case 10: Otto.crusaito(1, T, moveSize, -1);
      break;
    case 11: Otto.jump(1, T);
      break;
    case 12: Otto.flapping(1, T, moveSize, 1);
      break;
    case 13: Otto.flapping(1, T, moveSize, -1);
      break;
    case 14: Otto.tiptoeSwing(1, T, moveSize);
      break;
    case 15: Otto.bend(1, T, 1);
      break;
    case 16: Otto.bend(1, T, -1);
      break;
    case 17: Otto.shakeLeg(1, T, 1);
      break;
    case 18: Otto.shakeLeg(1, T, -1);
      break;
    case 19: Otto.jitter(1, T, moveSize);
      break;
    case 20: Otto.ascendingTurn(1, T, moveSize);
      break;
  }
}

void receiveGesture()
{
  Otto.home();
  int gesture = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) gesture = atoi(arg);
  else     delay(2000);
  switch (gesture) {
    case 1: Otto.playGesture(OttoHappy);
      break;
    case 2: Otto.playGesture(OttoSuperHappy);
      break;
    case 3: Otto.playGesture(OttoSad);
      break;
    case 4: Otto.playGesture(OttoSleeping);
      break;
    case 5: Otto.playGesture(OttoFart);
      break;
    case 6: Otto.playGesture(OttoConfused);
      break;
    case 7: Otto.playGesture(OttoLove);
      break;
    case 8: Otto.playGesture(OttoAngry);
      break;
    case 9: Otto.playGesture(OttoFretful);
      break;
    case 10: Otto.playGesture(OttoMagic);
      break;
    case 11: Otto.playGesture(OttoWave);
      break;
    case 12: Otto.playGesture(OttoVictory);
      break;
    case 13: Otto.playGesture(OttoFail);
      break;
    default: break;
  }
}

void receiveSing()
{
  Otto.home();
  int sing = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL) sing = atoi(arg);
  else     delay(2000);
  switch (sing) {
    case 1: Otto.sing(S_connection);
      break;
    case 2: Otto.sing(S_disconnection);
      break;
    case 3: Otto.sing(S_surprise);
      break;
    case 4: Otto.sing(S_OhOoh);
      break;
    case 5: Otto.sing(S_OhOoh2);
      break;
    case 6: Otto.sing(S_cuddly);
      break;
    case 7: Otto.sing(S_sleeping);
      break;
    case 8: Otto.sing(S_happy);
      break;
    case 9: Otto.sing(S_superHappy);
      break;
    case 10: Otto.sing(S_happy_short);
      break;
    case 11: Otto.sing(S_sad);
      break;
    case 12: Otto.sing(S_confused);
      break;
    case 13: Otto.sing(S_fart1);
      break;
    case 14: Otto.sing(S_fart2);
      break;
    case 15: Otto.sing(S_fart3);
      break;
    case 16: Otto.sing(S_mode1);
      break;
    case 17: Otto.sing(S_mode2);
      break;
    case 18: Otto.sing(S_mode3);
      break;
    case 19: Otto.sing(S_buttonPushed);
      break;
    default: break;
  }
}
