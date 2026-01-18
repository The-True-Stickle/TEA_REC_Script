#include <Servo.h> 

// Motor pins
const int motor1_in1 = 2;
const int motor1_in2 = 3;
const int motor1_pwm = 5;

//Button pins
const int operationButtonPin = 6;
int operationButtonState = 0;
const int eStopButtonPin = 7;
int eStopbuttonState = 0;

// Servo
Servo myServo;

//States
enum RideState {
    START_UP,
    RESTING,
    LOADING,
    RUNNING,
    UNLOADING,
    EMERGENCY_STOP
};

//Initial state at Start up
RideState currentState = START_UP;

//Consts
const int motorSpeed = 200; // PWM value for motor speed
const int rideDuration = 10000; // Duration of the ride in milliseconds
const int loadingDuration = 5000; // Duration of loading in milliseconds
const int unloadingDuration = 5000; // Duration of unloading in milliseconds
const int numCycles = 3; // Number of ride cycles

//Reference: https://forum.arduino.cc/t/using-millis-for-timing-a-beginners-guide/483573
unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds
bool currentPeriodState = false;  //initial state

void setup() {

    // Initialize motor pins
    pinMode(motor1_in1, OUTPUT);
    pinMode(motor1_in2, OUTPUT);
    pinMode(motor1_pwm, OUTPUT);

    // Initialize button pins
    pinMode(operationButtonPin, INPUT);
    pinMode(eStopButtonPin, INPUT);
    
    // Attach servo to pin 9
    myServo.attach(9);

    //Set initial state to resting after startup
    startMillis = millis();  //initial start time

}

void loop() {



    //Get the current time
    currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
    if (currentMillis - startMillis >= period && currentPeriodState == false)  //test whether the period has elapsed
    {
        startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
        currentPeriodState = true;
    }
    else {
        currentPeriodState = false;
    }


    //Check button states and update ride state accordingly
    operationButtonState = digitalRead(operationButtonPin);
    eStopbuttonState = digitalRead(eStopButtonPin);

    //If emergency stop button is pressed, go to emergency stop state
    if (eStopbuttonState == HIGH) {
        currentState = EMERGENCY_STOP;
        //Call emergency stop function
        emergencyStopState();
    }

    //If operating button is pressed, start cycle
    if (operationButtonState == HIGH) {

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

    //Do cool light effect function calls here

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
    default:
        break;
    }



}

void loadingState(bool initialize = false) {
    
    if (initialize) {
        //Do loading initialization operations here
    }

    //Transition to running state
    currentState = RUNNING;
    //Call running state function
    runningState();
}


void unloadingState() {

    //Do unloading operations here

    //Transition to resting state
    currentState = RESTING;
    //Call resting state function
    restingState();

}

void restingState() {

    //Do resting operations here

}

void emergencyStopState() {


    if (currentState != RESTING) {
        //Do emergency stop operations here (if it currently isn't in the rest state)

    }

    //Transition to resting state
    currentState = RESTING;
    //Call resting state function
    restingState();
}



void runningState() {

    //Do running operations here

    //Transition to unloading state
    currentState = UNLOADING;
    //Call unloading state function
    unloadingState();
}
