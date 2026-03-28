#include <Wire.h>


void setup() {
  Wire.begin(); // join i2c bus (address optional for master)

}

void loop() {
  // put your main code here, to run repeatedly:
  
  ChangeRideState();

  delay(1500);
}

void ChangeRideState() {
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write("Hi");        // sends five bytes
  Wire.endTransmission();    // stop transmitting

}
