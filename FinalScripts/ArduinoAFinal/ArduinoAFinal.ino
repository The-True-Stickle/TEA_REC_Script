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
int pistonPos = 0;

//Consts
const int rideDuration = 3600; // Duration of the ride in milliseconds
const int loadingDuration = 5000; // Duration of loading in milliseconds
const int unloadingDuration = 5000; // Duration of unloading in milliseconds

//Reference: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long ONE_SECOND = 1000;
const unsigned long FIVE_SECONDS = 5000;
const unsigned long FIFTEEN_SECONDS = 15000;
const unsigned long TEN_MILLISECONDS = 10;
const unsigned long HUNDRED_MILLISECONDS = 100;

enum RideState {
    START_UP,
    INITIALIZE_RIDE,
    DELAY,
    RESTING,
    LOADING,
    RUNNING,
    UNLOADING,
    EMERGENCY_STOP
};

//Initial state at Start up
RideState currentState = START_UP;

void setup() {
  
  //Connect the light strip to FastLED
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  //Connect the buttons to the pin numbers
  pinMode(startPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(restartPin), restartButton, HIGH);
  attachInterrupt(digitalPinToInterrupt(eStopPin), eStop, HIGH);

  //Connect the servo to the correct pin
  pistonServo.attach(pistonServoPin);

  //Sync the I2C Signal
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(067);
  delay(1000);
  Serial.print("666"); // All segments on - should show 8.8.8.8
  delay(2000);
  Serial.print("000"); // Should show 0000
  delay(2000);

}

void initializeRide() {
    //Do ride initialization operations here
    changeCurrentState(INITIALIZE_RIDE);

    //This one can prob use delay since it is not used during the actual ride cycles

    //After initialization, go to resting state
    changeCurrentState(RESTING);
}

//Change the ride state here so it can also send the signal to the child Arduino
void changeCurrentState(RideState newState) {
  //Update the state
  currentState = newState;

  //Send signal here

}

void loop() {
  //If ride is not initialized yet, initialize it
  if (currentState == START_UP) {
      initializeRide();
      return;
  }

  //Check button states and update ride state accordingly
  startButtonState = digitalRead(startPin);

  

}

void restartButton() {
  
}

void eStop() {
  changeCurrentState(EMERGENCY_STOP);
  //Call emergency stop function
  //emergencyStopState();
}
