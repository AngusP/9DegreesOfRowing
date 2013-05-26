/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *                   9DOF IMU Control Class
 *                          HEADER
 */

#include "Arduino.h"
#include <Wire.h>

class imu{
  
  public:
    imu(int VCC, int GND);
    void init();
    void power( boolean power );
    
    
  private:
    int _VCC;
    int _GND;
    
  
};

