#include <FastLED.h>
#include <Servo.h>
#include <Wire.h>

//Lights
#define LED_PIN   6
#define NUM_LEDS  64
CRGB leds[NUM_LEDS];
bool toggled = false;

//Buttons
const int startPin = 13;  
const int restartPin = 2;  
const int eStopPin = 3; 

int startButtonState = 0;

//Servo
Servo pistonServo;
const int pistonServoPin = 11;
//Angle 0 is the piston at resting state (ride in safe resting position for loading)
int pistonAngle = 0;

//Reference: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573
//Note: StartMillis are used for the main states while startMilliesLED is used for the lights
unsigned long startMillis;  
unsigned long startMillisLED;
const unsigned long ONE_SECOND = 1000;
const unsigned long FIVE_SECONDS = 5000;
const unsigned long FIFTEEN_SECONDS = 15000;
const unsigned long TEN_MILLISECONDS = 10;
const unsigned long HUNDRED_MILLISECONDS = 100;
const unsigned long TWENTY_FOUR_SECONDS = 2400;
const unsigned long TWENTY_SIX_SECONDS = 26000;
const unsigned long SERVO_LIFT_AND_LOWER_TIME = 100;

enum RideState {
    START_UP = 0,
    INITIALIZE_RIDE = 1,
    RESTING = 2,
    LOADING = 3,
    RUNNING = 4,
    UNLOADING = 5,
    RESTARTING = 6
};

enum RunningState {
  LIFTING,
  HOLDING,
  LOWERING
};
RunningState currentRunningState;

//Initial state at Start up
RideState currentState = START_UP;

//For LED Display
void DisplayLED(bool initialize = false) {
    if (initialize) {
      startMillisLED = millis();
      Serial.print("Beginning the LED Display\n");
    }


    switch(currentState) {
      case RESTING:
        //Display some peaceful calm lights
        break;
      case LOADING:
        //Display some more exciting lights
        break;
      case RUNNING:
        switch(currentRunningState) {
          case LIFTING:
            //Some wind up lights for lifting
            break;
          case HOLDING:
            //Some mind blowing crazy lights
            break;
          case LOWERING:
            //Wind down things. super cool looking lights
            break;

        }
        break;
      case UNLOADING:
        //Uh idk some calm-ish lights
        break;
      default:
        //Red lights to indicate not in standard operating state
        break;

      
    }
}

void setup() {

  //Open serial for debug
  Serial.begin(9600);

  Serial.print("\nSetup Procedure Started\n");

  Serial.print("Syncing the I2C Signal\n");
  //Sync the I2C Signal
  Wire.begin(8);             // join i2c bus with address #8
  Wire.onRequest(getState); // register event

  Serial.print("Connecting to the LED Lights\n");
  
  //Connect the light strip to FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  Serial.print("Connecting the buttons\n");

  //Connect the buttons to the pin numbers
  pinMode(startPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(restartPin), restartButton, HIGH);

  Serial.print("Connecting the servo\n");

  //Connect the servo to the correct pin
  pistonServo.attach(pistonServoPin);

  //Do ride initialization operations here
  changeCurrentState(INITIALIZE_RIDE);

  Serial.print("Moving servo to default position\n");
  //Ensure piston is in default spot
  pistonServo.write(0);
  pistonAngle = 0;

  Serial.print("Doing a brief connection pause\n");
  delay(1000);

  //After initialization, go to resting state
  changeCurrentState(RESTING);

  
  DisplayLED(true); 


}

void getState() {
  Wire.write(currentState);

}

//Change the ride state here so it can also send the signal to the child Arduino
void changeCurrentState(RideState newState) {
  //Update the state
  currentState = newState;

  //Print for debug
  /* REFERENCE
    START_UP = 0,
    INITIALIZE_RIDE = 1,
    RESTING = 2,
    LOADING = 3,
    RUNNING = 4,
    UNLOADING = 5,
    RESTARTING = 6
*/
  Serial.print("Current state is now: ");
  Serial.print(currentState);
  Serial.print("\n");
}






void restartRide(bool initialize = false) {
    if (initialize) {
        //Do unloading initialization operations here
        startMillis = millis();  //initial start time
        Serial.print("Ride Restart Initiated\n");
    }

    if (periodElapsed(startMillis, SERVO_LIFT_AND_LOWER_TIME * 2)) {
      pistonAngle -= 4;
      pistonServo.write(pistonAngle);

      //Reached Bottom
      if (pistonAngle <= 0) {
        //Ensure it's in the right position at the end
        pistonAngle = 0;
        pistonServo.write(pistonAngle);

        //Ride is safely in place for resting
        changeCurrentState(RESTING);
      }

    }
}


void restartButton() {
  //Call for an immediate ride restart
  changeCurrentState(RESTARTING);
  restartRide(true);
}


void unloadingState(bool initiaize = false) {

    if (initiaize) {
        //Do unloading initialization operations here
        startMillis = millis();  //initial start time
        Serial.print("Unloading state begins\n");
    }

    //Wait until load time is up
    if (periodElapsed(startMillis, TWENTY_FOUR_SECONDS)) {

      //Transition to resting state
      changeCurrentState(RESTING);
      


    }


  


}


void runningState(bool initialize = false) {

    if (initialize) {
        //Do running initialization operations here
        startMillis = millis();  //initial start time
        currentRunningState = LIFTING;
        Serial.print("Running State Begins\n");
        Serial.print("Running State: Lifting\n");
    }

          //If ride is still lifting, move the piston up more
    switch(currentRunningState) {
        case LIFTING:

          
          if (periodElapsed(startMillis, SERVO_LIFT_AND_LOWER_TIME)) {

            pistonAngle += 4;
            pistonServo.write(pistonAngle);

            //Serial.print(pistonAngle);
            //Serial.print(" ");

            //Reached Top
            if (pistonAngle >= 180) {
              currentRunningState = HOLDING;
              Serial.print("Running State: Holding\n");
            }
          }
          break;
        case HOLDING:
          //Keep it like that until ten seconds are up
          if (periodElapsed(startMillis, TWENTY_SIX_SECONDS)) {
            
            //Fun's over, time to lower the ride
            currentRunningState = LOWERING;
            Serial.print("Running State: Lowering\n");
          }
          break;
        case LOWERING:

          //Occurs every millisecond
          if (periodElapsed(startMillis, SERVO_LIFT_AND_LOWER_TIME)) {
            pistonAngle -= 4;
            pistonServo.write(pistonAngle);

            //Reached Top
            if (pistonAngle <= 0) {
              //Ensure it's in the right position at the end
              pistonAngle = 0;
              pistonServo.write(pistonAngle);

              //Ride is over, begin unload
              changeCurrentState(UNLOADING);
              unloadingState(true);
            }

          }


          break;
    }


}

void loadingState(bool initialize = false) {


    if (initialize) {
        //Do loading initialization operations here
        startMillis = millis();  //initial start time
        Serial.print("Loading State Begins\n");
    }


    //Wait until load time is up
    if (periodElapsed(startMillis, TWENTY_FOUR_SECONDS)) {

      //Transition to running state
      changeCurrentState(RUNNING);
      //Call running state function
      runningState(true);


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

//Since the loop needs to be called repeatedly, we need a function to continue the current state
void continueFunction() {
    switch (currentState)
    {
        case LOADING:
            loadingState();
            break;
        case RUNNING:
            runningState();
            break;
        case UNLOADING:
            unloadingState();
            break;
        case RESTARTING:
            restartRide();
        default:
            break;
    }



}

void loop() {

  //Check button states and update ride state accordingly
  startButtonState = digitalRead(startPin);

    for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB ( 0, 0, 255);
        FastLED.show();
    }
    FastLED.show();

  //If operating button is pressed, start cycle
  if (startButtonState == HIGH) {

      //If the current state is resting, go to loading state
      if (currentState == RESTING) {

          //Set current state to loading
          changeCurrentState(LOADING);
          //Call loading state function
          loadingState(true);
          return;
      }
  }
  
  DisplayLED();
  continueFunction();

}



