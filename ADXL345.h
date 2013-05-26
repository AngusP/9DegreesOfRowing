/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *           ADXL245 Accelerometer Interface Library
 *                           HEADER
 */

#include "Arduino.h"
#include <Wire.h>

class adxl{
  
  public:
    adxl();
    void init();
    byte requestByte( char dir );
    int getAccel( char dir );
    
    
  private:
    // Accelerometer Address
    int ADXL_ADDR;
    // Init Instructions
    int POWER_CTL;
    int MEASURE;
    int FORMAT;
    int MODE;
    // Data register addresses
    int X0;
    int X1;
    int Y0;
    int Y1;
    int Z0;
    int Z1;
    // getDir variable
    int var;
  
};
