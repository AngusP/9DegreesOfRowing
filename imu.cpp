/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *                   9DOF IMU Control Class
 *                         SOURCE FILE
 */


#include "imu.h"


// Run the constructor
imu::imu(int VCC, int GND){
  
  pinMode(VCC, OUTPUT);
  pinMode(GND, OUTPUT);
  
  // Copy public vars to private vars
  _VCC = VCC;
  _GND = GND;
  
}


void imu::init(){
  
  Wire.begin();
  digitalWrite(_VCC, HIGH);
  digitalWrite(_GND, LOW);
  
}


void imu::power( boolean power ){
  
  digitalWrite(_VCC, power);
  
}


void imu::readConfig(){
  
  // May move readConfig() here from main .ino
  
}
