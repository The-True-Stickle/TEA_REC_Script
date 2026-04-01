#include <Wire.h>
#include <AFMotor_R4.h>

//For simplicity, the current state is handled by an int
int currentState = 0;
bool currentlyRunning = false;

/* REFERENCE
    START_UP = 0,
    INITIALIZE_RIDE = 1,
    RESTING = 2,
    LOADING = 3,
    RUNNING = 4,
    UNLOADING = 5,
    RESTARTING = 6
*/

// Motor pin connections
const int directionPin = 2; // direction pin
const int stepperPin = 3; // step pin

//Reference: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573
unsigned long startMillisMotor;
unsigned long startMillisOperation;  

//For motor rotation
unsigned int currentMicroDelay = 0;
//Note: In MICROS
const unsigned long MINIMUM_MICRO_DELAY = 1000;
const unsigned long MAXIMUM_MICRO_DELAY = 5000;
const unsigned long SIXTEEN_SECONDS = 16000;
//Other times
const unsigned long ONE_SECOND = 1000;


enum RunningState {
  SPEEDING_UP,
  HOLDING,
  SLOWING_DOWN
};
RunningState currentRunningState;

void setup() {

  pinMode(directionPin, OUTPUT);
  pinMode(stepperPin, OUTPUT);
  // set direction of rotation to clockwise
  digitalWrite(directionPin, LOW);

  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);

  Serial.print("Done initializing\n");

  
}

void runStandardOperation(bool initiaize = false) {

  if (initiaize) {
      startMillisOperation = millis();  //initial start time
      startMillisMotor = millis();
      Serial.print("Starting new standard operation\n");
      Serial.print("Motor state: SPEEDING_UP\n");
      currentMicroDelay = MAXIMUM_MICRO_DELAY;
      currentRunningState = SPEEDING_UP;
  }

  switch(currentRunningState) {
    case SPEEDING_UP:
      if (periodElapsed(startMillisMotor, 100)) {
        currentMicroDelay -= (int)((MAXIMUM_MICRO_DELAY - MINIMUM_MICRO_DELAY) / 100);
      }
      


      //Check if it's time to stop the speeding up phase
      if (currentMicroDelay <= MINIMUM_MICRO_DELAY) {
          currentMicroDelay = MINIMUM_MICRO_DELAY;
          currentRunningState = HOLDING;
          Serial.print("Motor state: HOLDING\n");
      }

      //Actually move the motor
      digitalWrite(stepperPin, HIGH);
      delayMicroseconds(currentMicroDelay);
      digitalWrite(stepperPin, LOW);
      delayMicroseconds(currentMicroDelay);  
      break;

    case HOLDING:

      //Check if the motor has been spinning for the necessary 16 seconds
      if (periodElapsed(startMillisMotor, SIXTEEN_SECONDS)) {
        currentRunningState = SLOWING_DOWN;
        Serial.print("Motor state: SLOWING_DOWN\n");
      }

      //Actually move the motor
      digitalWrite(stepperPin, HIGH);
      delayMicroseconds(currentMicroDelay);
      digitalWrite(stepperPin, LOW);
      delayMicroseconds(currentMicroDelay);  
      break;

    case SLOWING_DOWN:
      
      if (periodElapsed(startMillisMotor, 100)) {
        currentMicroDelay += (int)((MAXIMUM_MICRO_DELAY - MINIMUM_MICRO_DELAY) / 100);
      }
      
      //Don't let it get too slow before the change of state
      if (currentMicroDelay >= MAXIMUM_MICRO_DELAY) {
          currentMicroDelay = MAXIMUM_MICRO_DELAY;
          //Keep the speed here until it's done
      }

      //Actually move the motor
      digitalWrite(stepperPin, HIGH);
      delayMicroseconds(currentMicroDelay);
      digitalWrite(stepperPin, LOW);
      delayMicroseconds(currentMicroDelay);  


      break;

  }

  
}

void loop() {

  Wire.requestFrom(8, 1);    // request 1 byte from peripheral device #8

  while (Wire.available()) { // peripheral may send less than requested
    int newState = Wire.read(); // receive a byte as character
    if (currentState != newState) {
      currentState = newState;        // print the character
      Serial.print("New State Found: ");
      Serial.print(currentState);
      Serial.print("\n");
      if (currentState == 4 && !currentlyRunning) {
        runStandardOperation(true);
        currentlyRunning = true;
        return;
      }
      else {
        //Halt all movement
        digitalWrite(stepperPin, LOW);
        currentlyRunning = false;
      }
    }
    

    

  }

  if (currentState == 4) {
    runStandardOperation();
  }
  
  

}



//Utility function for timing using millis()
bool periodElapsed(unsigned long &previousMillis, const unsigned long interval) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}

