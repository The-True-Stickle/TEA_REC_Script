#include <Wire.h>




void setup() {
  // put your setup code here, to run once:
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);     
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
}


void receiveEvent(int howMany)
{

  char[] Message
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Message += c;
    Serial.print(c);         // print the character
  }

  Serial.print(Message);


}