/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *           ADXL245 Accelerometer Interface Library
 *                         SOURCE FILE
 */


#include "ADXL345.h"



// Constructor
adxl::adxl(){
  
  // Accelerometer Address
  ADXL_ADDR = 0x53;
  // Init Instructions
  POWER_CTL = 0x2D;// POWER_CTL
  MEASURE   = 0x09;// measurement mode, 4hz wakeup
  FORMAT    = 0x31;// DATA_FORMAT
  MODE      = 0x03;// Go into 16g mode
  // Data register addresses
  X0 = 0x32;
  X1 = 0x33;
  Y0 = 0x34;
  Y1 = 0x35;
  Z0 = 0x36;
  Z1 = 0x37;
  
}



// Initialisation code:
void adxl::init(){
  
  Wire.beginTransmission(ADXL_ADDR); // transmit to ADXL345
  Wire.write(POWER_CTL);// POWER_CTL
  Wire.write(MEASURE);// measurement mode, 4hz wakeup
  Wire.write(FORMAT);// DATA_FORMAT
  Wire.write(MODE);// Go into 16g mode
  Wire.endTransmission(); // stop transmitting
  
}


// Request a byte of information:
byte adxl::requestByte( char dir ){
  Wire.beginTransmission(ADXL_ADDR); // transmit to ADXL345
  Wire.write(dir);
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(ADXL_ADDR, 1); // request 1 byte from ADXL345
  while(Wire.available())
  {
    return(Wire.read()); 
  }
}


// Get acceleration for a direction
int adxl::getAccel(char dir){
  int var;

  if(dir=='x'){
    var=requestByte(X0);
    var=var+(requestByte(X1)<<8);
  }
  else if(dir=='y'){
    var=requestByte(Y0);
    var=var+(requestByte(Y1)<<8);
  }
  else if(dir=='z'){
    var=requestByte(Z0);
    var=var+(requestByte(Z1)<<8);
  }
  
  return(var);
}
