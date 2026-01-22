#include <Servo.h> 

// Motor pins
const int motor1_in1 = 2;
const int motor1_in2 = 3;
const int motor1_pwm = 5;

//Button pins
const int startButtonPin = 6;
int startButtonState = 0;
const int eStopButtonPin = 7;
int eStopbuttonState = 0;

// Servo
//Servo myServo;

//States
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

//Consts (example)
const int motorSpeed = 200; // PWM value for motor speed
const int rideDuration = 10000; // Duration of the ride in milliseconds
const int loadingDuration = 5000; // Duration of loading in milliseconds
const int unloadingDuration = 5000; // Duration of unloading in milliseconds
const int numCycles = 3; // Number of ride cycles

//Reference: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long ONE_SECOND = 1000;
const unsigned long FIVE_SECONDS = 5000;
const unsigned long TEN_MILLISECONDS = 10;
const unsigned long HUNDRED_MILLISECONDS = 100;
const unsigned long FIFTEEN_SECONDS = 15000;


void setup() {

    // Initialize motor pins
    pinMode(motor1_in1, OUTPUT);
    pinMode(motor1_in2, OUTPUT);
    pinMode(motor1_pwm, OUTPUT);

    // Initialize button pins
    pinMode(startButtonPin, INPUT);
    pinMode(eStopButtonPin, INPUT);

    // Attach servo to pin 9
    myServo.attach(9);


}

void initializeRide() {
    //Do ride initialization operations here

    currentState = INITIALIZE_RIDE;

    //This one can prob use delay since it is not used during the actual ride cycles
    

    //After initialization, go to resting state
    currentState = RESTING;
}

void loop() {

    //If ride is not initialized yet, initialize it
    if (currentState == START_UP) {
        initializeRide();
        return;
    }

    //Check button states and update ride state accordingly
    startButtonState = digitalRead(startButtonPin);
    eStopbuttonState = digitalRead(eStopButtonPin);

    //If emergency stop button is pressed, go to emergency stop state
    if (eStopbuttonState == HIGH) {
        currentState = EMERGENCY_STOP;
        //Call emergency stop function
        emergencyStopState();
    }

    //If operating button is pressed, start cycle
    if (startButtonState == HIGH) {

        //If the current state is resting, go to loading state
        if (currentState == RESTING) {

            //Set current state to loading
            currentState = LOADING;
            //Call loading state function
            loadingState(true);
            return;
        }
    }



    //Continue current state operations
    continueFunction();

    //Do cool light effect function calls or other stuff here

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
    case EMERGENCY_STOP:
        emergencyStopState();
        break;
    case DELAY:
        delayState();
        break;
    default:
        break;
    }



}

void loadingState(bool initialize = false) {


    if (initialize) {
        //Do loading initialization operations here
        startMillis = millis();  //initial start time
    }

    //Occurs every ten milliseconds
    if (periodElapsed(startMillis, TEN_MILLISECONDS)) {

        //Do loading operations like motor movements here


    }




    if (true /*loading completed*/) {

        //Transition to running state
        currentState = RUNNING;
        //Call running state function
        runningState(true);
    }

}


void unloadingState(bool initiaize = false) {

    if (initialize) {
        //Do unloading initialization operations here
        startMillis = millis();  //initial start time
    }

    //Occurs every ten milliseconds
    if (periodElapsed(startMillis, TEN_MILLISECONDS)) {

        //Do unloading operations like motor movements here


    }




    if (true /*unloading completed*/) {

        //Transition to delay state
        currentState = DELAY;
        //Call delay state function
        delayState();
    }

}


void emergencyStopState() {


    //Uhh idk yet but it prob involves stopping motors and stuff


}



void runningState(bool initialize = false) {

    if (initialize) {
        //Do running initialization operations here
        startMillis = millis();  //initial start time
    }

    //Occurs every ten milliseconds
    if (periodElapsed(startMillis, TEN_MILLISECONDS)) {

        //Do running operations like motor movements here


    }




    if (true /*running completed*/) {

        //Transition to unloading state
        currentState = UNLOADING;
        //Call unloading state function
        unloadingState(true);
    }
}

void delayState() {

    //Utility function to create delays without blocking
    startMillis = millis();  //initial start time
    if (periodElapsed(startMillis, FIFTEEN_SECONDS)) {
        //Delay completed, begin loading again
        currentState = LOADING;
        loadingState(true);
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
