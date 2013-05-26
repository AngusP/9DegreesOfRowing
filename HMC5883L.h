/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *           HMC588L Magnetometer Interface Library
 *                          HEADER
 */

#include "Arduino.h"
#include <Wire.h>

class hmc{
  
  public:
    hmc();
    void init();
    int getMag( char dir );
    
  private:
    int HMC_ADDR;
    int CONF_A, CONF_B;
    int MODE;
    int X0;
    int X1;
    int Y0;
    int Y1;
    int Z0;
    int Z1;
  
};

