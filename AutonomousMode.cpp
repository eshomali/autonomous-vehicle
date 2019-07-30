
#include <SPI.h>  
#include <Pixy.h>
#include <Servo.h>
#include <inttypes.h>
#include <SoftwareSerial.h>

Servo servoPan;
Servo servo;
Servo escServoR;
Servo escServoL;
Pixy pixy;

const int servoPinPan = 9;
const int servoPinTilt = 10;

int speedVar = 0;
int speedVal;
int speedMultiplier;
int leftSpeed;
int rightSpeed;

int servoValPan = 90;
int servoMovePan = 0;
int prevServoMovePan = 0;
int prevServoValPan = 0;
int scanCount;
int scanState = 0;
int totalServoValPan[5] = {90,90,90,90,90};
int finalServoValPan;

int servoValTilt = 125;
int servoMoveTilt = 0;
int prevServoMoveTilt = 0;
int prevServoValTilt = 0;
int totalServoValTilt[3] = {125,125,125};
int finalServoValTilt;

int startUpError = 0;

byte widthByte;
byte servoByte;

int signalLost;
int signalCount;

int servoScan = 0;

int scanVal = 0;

//int signalLost;
//int signalCount;

//int servoScan = 0;

//int scanVal = 0;

void setup()
{

  Serial.begin(115200);
  Serial.print("Starting...\n");
  servoPan.attach(servoPinPan);
  servo.attach(servoPinTilt);
  servoPan.write(servoValPan);
  servo.write(servoValTilt);
  pixy.init();

  escServoL.attach(4);
  escServoR.attach(2);
  
}

void loop(){ 
  int i,j,k,p;
  uint16_t blocks;
  int32_t tempX, tempY, tempWidth, tempHeight;
  int xVal, yVal, widthVal, heightVal;
  char buf[32];
  
  blocks = pixy.getBlocks();
  
  tempX = pixy.blocks[j].x;
  tempY = pixy.blocks[j].y;

  tempWidth = pixy.blocks[j].width;
  tempHeight = pixy.blocks[j].height;
  
  xVal = tempX;
  yVal = tempY; 

  widthVal = tempWidth;
  heightVal = tempHeight;
  
  if (blocks) {

    Serial.println("Con");

    signalLost = 0;
    signalCount = 0;
    
    servoMovePan = map(xVal, 0, 159, 37, 0);      //mapping the left half of the viewing plane to a degree value between 0 and 37 degree movement.
    servoMovePan = map(xVal, 160, 160, 0, 0);     //mapping the center of the x axis to no movement
    servoMovePan = map(xVal, 161, 320, 0, -37);   //mapping the right half of the viewing plane to a degree value between 0 and -37 degree movement.

    servoMoveTilt = map(yVal, 0, 99, 21, 0);      //mapping the top half of the viewing plane to a degree value between 0 and 21 degree movement.
    servoMoveTilt = map(yVal, 100, 100, 0, 0);    //mapping the center of the y axis to no movement
    servoMoveTilt = map(yVal, 101, 200, 0, -21);  //mapping the bottom half of the viewing plane to a degree value between 0 and -21 degree movement.
       
    servoValPan = servoValPan + servoMovePan;     

    servoValTilt = servoValTilt + servoMoveTilt;

    
    if (servoValPan > 390){
      servoValPan = 390;
    }
    else if(servoValPan < -208){
      servoValPan = -208;
    }
    totalServoValPan[k] = servoValPan;
    k++;
    if (k == 4) k = 0;

    if (servoValTilt > 320){
      servoValTilt = 320;
    }
    else if(servoValTilt < -40){
      servoValTilt = -40;
    }
    totalServoValTilt[p] = servoValTilt;
    p++;
    if (p == 2) p = 0; 

    finalServoValPan = (totalServoValPan[0] + totalServoValPan[1] + totalServoValPan[2] + totalServoValPan[3] + totalServoValPan[4]) / 5;  

    finalServoValTilt = (totalServoValTilt[0] + totalServoValTilt[1] + totalServoValTilt[2]) / 3;  

    
    if (finalServoValPan > 150) finalServoValPan = 150;
    if (finalServoValPan < 30) finalServoValPan = 30;
  
    if (finalServoValTilt > 160) finalServoValTilt = 160;
    if (finalServoValTilt < 90) finalServoValTilt = 90; 

    servo.write(finalServoValTilt);
  
    servoPan.write(finalServoValPan);

    if (widthVal <= 25){
      speedVal = map(widthVal, 25, 5, 98, 130);
    }
    else if (widthVal >= 35){
      speedVal = map(widthVal, 35, 250, 82, 50);
    }
    else{
      speedVal = 90;
    }

    if(speedVal > 90){                                          //if the object is moving forward
  
      speedVar = speedVal - 90;                                 //getting the shifted value of speed
      
      if(finalServoValPan > 90){                                    //if the desired direction is left
        speedMultiplier = map(finalServoValPan, 150, 90, 0, 100);  //setting the speed multiplier. 
        leftSpeed = ((speedVar * speedMultiplier)/100) + 90;      //when the pan servo is at between 150 and 90 degrees, 
        rightSpeed = speedVal;                                  //the speed of the left motor will be going slower than the right
      }
      else if(finalServoValPan < 90){                               //if the desired direction is right
        speedMultiplier = map(finalServoValPan, 90, 30, 100, 0);    //setting the speed multiplier. 
        rightSpeed = ((speedVar * speedMultiplier)/100) + 90;     //when the pan servo is at between 90 and 0 degrees, 
        leftSpeed = speedVal;                                   //the speed of the right motor will be going slower than the left
      }
      else if (finalServoValPan = 90){                              //if the desired direction is straight forward
        leftSpeed = speedVal;                                   //both motors will be turning at the same speed
        rightSpeed = speedVal;
      }
    }
    else if (speedVal < 90){                                   //if the object is moving backward
      
      speedVar = 90 - speedVal;                                 //shifting the value of the speed
  
      if(finalServoValPan > 90){                                    //if the desired direction is left
        speedMultiplier = map(finalServoValPan, 150, 90, 0, 100);  //setting the speed multiplier.
        leftSpeed = 90 - ((speedVar * speedMultiplier)/100);     //when the pan servo is at between 150 and 90 degrees, 
        rightSpeed = speedVal;                                  //the speed of the left motor will be going slower than the right
      }
      else if(finalServoValPan < 90){                               //if the desired direction is right
        speedMultiplier = map(finalServoValPan, 90, 30, 100, 0);    //setting the speed multiplier. 
        rightSpeed = 90 - ((speedVar * speedMultiplier)/100);    //when the pan servo is at between 90 and 0 degrees, 
        leftSpeed = speedVal;                                   //the speed of the right motor will be going slower than the left
      }
      else if (finalServoValPan = 90){                              //if the desired direction is straight backwards
        leftSpeed = speedVal;                                   //both motors will be turning at the same speed
        rightSpeed = speedVal;
      }
    }
    else{                             //if the desired direction is straight backwards
        leftSpeed = 90;                                   //both motors will be turning at the same speed
        rightSpeed = 90;
    }

    escServoL.write(leftSpeed);
    escServoR.write(rightSpeed);
  }
  else{
  
    if(signalCount > 500){
      Serial.println("Not Con");
      signalLost = 1;
      escServoL.write(90);
      escServoR.write(90);
    }
    else if(signalLost == 0){
      signalCount++;
    }
//    else if(signalLost == 1){
//      if(scanVal == 0){
//        servoPan.write(servoScan);
//        servoScan++;
//        if(servoScan == 181){
//          scanVal = 1;
//          servoScan = 180;
//        }
//      }
//      else if(scanVal == 1){
//        servoPan.write(servoScan);
//        servoScan--;
//        if(servoScan == -1){
//          scanVal = 0;
//          servoScan = 0;
//        }        
//      }
//    } 
  }
 }
