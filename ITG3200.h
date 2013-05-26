/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *             ITG3200 Gyroscope Interface Library
 *                          HEADER
 */

#include "Arduino.h"
#include <Wire.h>

class itg{
  
  public:
    // Constructor:
    itg();
    void init();
    int getGyro( char dir );
    int gx, gy, gz;
    // Most significant and least significant bytes
    byte msb, lsb;
    
    
  private:
    // Gyro address, binary = 11101001
    int ITG_ADDR;
    // Init Instructions
    int SMPLRT_DIV;
    int DLPF_FS;
    int INT_CFG;
    int PWR_MGM;
  
};

