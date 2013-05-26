/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *             ITG3200 Gyroscope Interface Library
 *                        SOURCE FILE
 */


#include "ITG3200.h"


// Constructor:
itg::itg(){

  ITG_ADDR   = 0x68;
  // Init Instructions
  SMPLRT_DIV = 0x02;
  DLPF_FS    = 0x16;
  INT_CFG    = 0x17;
  PWR_MGM    = 0x3E;

}



// Initialisation code:
void itg::init(){

  /*
   ITG 3200
   power management set to:
   clock select = internal oscillator
   no reset, no sleep mode
   no standby mode
   sample rate to = 500Hz
   parameter to +/- 2000 degrees/sec
   low pass filter = 5Hz
   no interrupt
   */

  Wire.beginTransmission(ITG_ADDR);
  Wire.write(PWR_MGM);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(ITG_ADDR);
  Wire.write(SMPLRT_DIV);
  Wire.write(0xFF); // EB, 50, 80, 7F, DE, 23, 20, FF
  Wire.endTransmission();

  Wire.beginTransmission(ITG_ADDR);
  Wire.write(DLPF_FS);
  Wire.write(0x1E); // +/- 2000 dgrs/sec, 1KHz, 1E, 19
  Wire.endTransmission();

  Wire.beginTransmission(ITG_ADDR);
  Wire.write(INT_CFG);
  Wire.write(0x00);
  Wire.endTransmission();

}

int itg::getGyro( char dir ){

  /**************************************
   * 		Gyro ITG-3200 I2C
   * 		registers:
   * 		x axis MSB = 1D, x axis LSB = 1E
   * 		y axis MSB = 1F, y axis LSB = 20
   * 		z axis MSB = 21, z axis LSB = 22
   	**************************************/

  byte msb = 0;
  byte lsb = 0;

  switch ( dir ) {

  case 'x':
  
    // clear variables
    msb = 0;
    lsb = 0;
    
    // Arduino Wire library (I2C)
    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x1D); // MSB x axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte
    while(!Wire.available());
    msb = Wire.read();

    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x1E); // LSB x axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte

    while(!Wire.available());
    lsb = Wire.read();

    // calculate total x axis
    return gx = (( msb << 8) | lsb);

    break;

  case 'y':
    
    // clear variables
    msb = 0;
    lsb = 0;
    
    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x1F); // MSB y axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte

    while(!Wire.available());
    msb = Wire.read();

    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x20); // LSB y axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte

    while(!Wire.available());
    lsb = Wire.read();

    // calculate total y axis
    return gy = (( msb << 8) | lsb);

    break;

  case 'z':

    // clear variables
    msb = 0;
    lsb = 0;
    
    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x21); // MSB z axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte

    while(!Wire.available());
    msb = Wire.read();

    Wire.beginTransmission(ITG_ADDR);
    Wire.write(0x22); // LSB z axis
    Wire.endTransmission();
    Wire.requestFrom(ITG_ADDR, 1); // one byte

    while(!Wire.available());
    lsb = Wire.read();

    // calculate z axis
    return gz = (( msb << 8) | lsb);

    break;

  default:

    return 0;

    break;

  }

}



