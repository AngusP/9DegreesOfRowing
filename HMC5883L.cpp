/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *           HMC588L Magnetometer Interface Library
 *                         SOURCE FILE
 */


#include "HMC5883L.h"


// Constructor
hmc::hmc(){
  
  // Magnetometer Address
  HMC_ADDR = 0x1E;
  CONF_A = 0x00;
  CONF_B = 0x01;
  MODE = 0x02;
  
  // Data register addresses
  X0 = 0x03;
  X1 = 0x04;
  Y0 = 0x07;
  Y1 = 0x08;
  Z0 = 0x05;
  Z1 = 0x06;
  
}



// Initialisation code:
void hmc::init(){
  
  Wire.beginTransmission(HMC_ADDR);
  Wire.write(MODE);
  Wire.write(0x00); //Continuous measurement
  Wire.endTransmission();
  
}






int hmc::getMag( char dir ){
  
  int mx, my, mz;
  
  Wire.beginTransmission(HMC_ADDR);
  Wire.write(X0); //select register 3, X MSB register
  Wire.endTransmission();
  
 
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(HMC_ADDR, 6);
  if(6<=Wire.available()){
    mx = Wire.read()<<8; //X msb
    mx |= Wire.read(); //X lsb
    mz = Wire.read()<<8; //Z msb
    mz |= Wire.read(); //Z lsb
    my = Wire.read()<<8; //Y msb
    my |= Wire.read(); //Y lsb
  }
  
  if( dir == 'x' ){
    return mx;
  } else if( dir == 'y' ){
    return my;
  } else if( dir == 'z' ){
    return mz;
  }
  
}
