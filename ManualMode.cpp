#include <PS3BT.h>
#include <usbhub.h>
#include <Servo.h>

int connectCount;

int servoValPan = 90;
int servoMovePan = 0;
int prevServoMovePan = 0;
int prevServoValPan = 0;
int scanCount;
int scanState = 0;
int totalServoValPan[5] = {90,90,90,90,90};

int widthVal = 0;
int servoVal = 0;

Servo escServoR;
Servo escServoL;

Servo servoTest;

int leftSpeed;
int rightSpeed;

int speedVal;
int directionVal;

byte widthByte;
byte servoByte;

int speedMultiplier;

int speedVar = 0;

int mode;

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  
  #if !defined(__MIPSEL__)
    Serial.println("serial");
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
    
  #endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }

  pinMode(7, OUTPUT);
  
  pinMode(8, OUTPUT); //G 
  pinMode(9, OUTPUT); //Y
  pinMode(5, OUTPUT); //R 


  escServoL.attach(4);
  escServoR.attach(2);

  servoTest.attach(A0);

  mode = 1;
}

void loop() {

  servoTest.write(90);
  
  Usb.Task();

  int leftX = PS3.getAnalogHat(LeftHatX);
  int leftY = PS3.getAnalogHat(LeftHatY);
  int rightX = PS3.getAnalogHat(RightHatX);
  int rightY = PS3.getAnalogHat(RightHatY);

//  Serial.print(widthVal);
//  Serial.print("  ");
//  Serial.print(digitalRead(7));
//  Serial.print(digitalRead(6));
//  Serial.print(digitalRead(5));
//  Serial.print(digitalRead(4));
//  Serial.print(digitalRead(3));
//  Serial.print(digitalRead(2));
//  Serial.print("  ");
//  Serial.println(servoVal);


  Serial.print("connect: ");
  Serial.println(connectCount);
  
  if (PS3.PS3Connected || PS3.PS3NavigationConnected){

    connectCount = 0;

    if (PS3.getButtonClick(PS)) {
      PS3.disconnect();
    }
    else {
      if (PS3.getButtonClick(SELECT)) {

      }
      if (PS3.getButtonClick(START)) {

      }
      if (PS3.getButtonClick(TRIANGLE)) {
        //Serial.println("UP, HIGH");
      }
      if (PS3.getButtonClick(CIRCLE)) {
        //Serial.println("RIGHT, LOW");
        mode = 0;
      }
      if (PS3.getButtonClick(SQUARE)) {
        //Serial.println("LEFT, HIGH");
        mode = 1; 
      }
      if (PS3.getButtonClick(CROSS)) {
        //Serial.println("DOWN, LOW");
      }
      if (PS3.getButtonClick(MOVE)) {

      }
    }
    
  
    if(mode == 0){ // Autonomous
      digitalWrite(8, HIGH); // GREEN
      digitalWrite(9, LOW);
      digitalWrite(5, LOW);

      digitalWrite(7, HIGH);
    }
    
    else if(mode == 1){ // Manual
      digitalWrite(8, LOW); 
      digitalWrite(9, HIGH); // YELLOW
      digitalWrite(5, LOW);

      digitalWrite(7, LOW);
      
      //Serial.print("PS3 Mode");
      //int speedVal = map(rightY, 10, 120, 110, 70); Use this for autonomous mode
      if (rightY < 117){
        speedVal = map(rightY, 117, 0, 90, 180);
      }
      else if (rightY > 137){
        speedVal = map(rightY, 137, 255, 90, 0);
      }
      else{
        speedVal = 90;
      }
  
       // Setting the direction of turning for the vehicle. directionVal is already set up for pan servo integration
      if (leftX <117 ){
        directionVal = map(leftX, 117, 0, 90, 150);
      }
      else if (leftX > 137){
        directionVal = map(leftX, 137, 255, 90, 30);
      }
      else{
        directionVal = 90;
      }
      speedVar = 0;
  
      if(speedVal > 90){                                          //if the object is moving forward
    
        speedVar = speedVal - 90;                                 //getting the shifted value of speed
        
        if(directionVal > 90){                                    //if the desired direction is left
          speedMultiplier = map(directionVal, 150, 91, 0, 80);  //setting the speed multiplier. 
          leftSpeed = ((speedVar * speedMultiplier)/100) + 90;      //when the pan servo is at between 150 and 90 degrees, 
          rightSpeed = speedVal;                                  //the speed of the left motor will be going slower than the right
        }
        else if(directionVal < 90){                               //if the desired direction is right
          speedMultiplier = map(directionVal, 91, 30, 80, 0);    //setting the speed multiplier. 
          rightSpeed = ((speedVar * speedMultiplier)/100) + 90;     //when the pan servo is at between 90 and 0 degrees, 
          leftSpeed = speedVal;                                   //the speed of the right motor will be going slower than the left
        }
        else if (directionVal = 90){                              //if the desired direction is straight forward
          leftSpeed = speedVal;                                   //both motors will be turning at the same speed
          rightSpeed = speedVal;
        }
      }
      else if ( speedVal < 90){                                   //if the object is moving backward
        
        speedVar = 90 - speedVal;                                 //shifting the value of the speed
    
        if(directionVal > 90){                                    //if the desired direction is left
          speedMultiplier = map(directionVal, 150, 90, 0, 100);  //setting the speed multiplier.
          leftSpeed = 90 - ((speedVar * speedMultiplier)/100);     //when the pan servo is at between 150 and 90 degrees, 
          rightSpeed = speedVal;                                  //the speed of the left motor will be going slower than the right
        }
        else if(directionVal < 90){                               //if the desired direction is right
          speedMultiplier = map(directionVal, 90, 30, 100, 0);    //setting the speed multiplier. 
          rightSpeed = 90 - ((speedVar * speedMultiplier)/100);    //when the pan servo is at between 90 and 0 degrees, 
          leftSpeed = speedVal;                                   //the speed of the right motor will be going slower than the left
        }
        else if (directionVal = 90){                              //if the desired direction is straight backwards
          leftSpeed = speedVal;                                   //both motors will be turning at the same speed
          rightSpeed = speedVal;
        }
      }
      else{
        leftSpeed = 90;                                           //if the vehicle is the correct distance from the object/not needed to move,
        rightSpeed = 90;                                          //both motors will be set to stand still
      } 
      
      escServoL.write(leftSpeed);
      escServoR.write(rightSpeed);
    }
    
  }
  else{
    
    connectCount++;
    if(connectCount > 500){
      
      digitalWrite(8, LOW); 
      digitalWrite(9, LOW);
      digitalWrite(5, HIGH); // RED
    }

    escServoL.write(90);
    escServoR.write(90);  
  }
}


