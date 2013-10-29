/*
 *  I M U   -   I N E R T I A L    M E A S U R E M E N T    U N I T
 *
 *        Arduino Mega Pro 5v and SFE 9Dof Sensor Stick
 *
 */

#include <Wire.h>

#include "ADXL345.h"
#include "HMC5883L.h"
#include "ITG3200.h"
#include "imu.h"

// store error strings in flash to save RAM
//#define error(s) error_P(PSTR(s))

//SDA is pin 20, SCL is pin 21
//VCC is pin 18 and GND pin 19


const int NEWF = 30; // Stop and restart serial connection (Useful for OpenLog)
const int RATE_U = 36; // Button to increase recording speed
const int RATE_D = 42; // Button to decrease speed
const int TOGGLE = 48; // Button input to start/stop recording

unsigned long startTime = 0; // Time to subtract from millis()

// Initialise classes
imu imu(18, 19);
adxl adxl;
hmc hmc;
itg itg;

unsigned int maxRate = 2000; // Maximum rate. 0.5Hz
unsigned int minRate = 40; // Minimum time period for logging. 25Hz
unsigned int rate = minRate; // Initial time period is fastest possible by default
int frequency = 1000/rate; // 1000ms in 1 second

boolean notify = 0; //Notification LED state
boolean toggleState = 0;
boolean rateUState = 0;
boolean rateDState = 0;
unsigned int rateInc = 10; // milliseconds to increase rate by each time

unsigned long lastBounce = 0;  //debouncing control
int bounceThreshold = 250; // 1/4 of a second

// More stable timing between readings:
long then = 0;
long now = 0;
long waitTime = 0;

int ax, ay, az;
int gx, gy, gz;
int mx, my, mz;

byte buffer; // Setting read buffer

// Default settings
boolean logax = true;
boolean logay = true;
boolean logaz = true;
boolean loggx = true;
boolean loggy = true;
boolean loggz = true;
boolean logmx = true;
boolean logmy = true;
boolean logmz = true;
unsigned long logfor = 0; // 0 to 9999000 milliseconds
unsigned long logperiod = 0; // 0 to 9999000 milliseconds






void setup() {

  pinMode(TOGGLE, INPUT);
  pinMode(RATE_U, INPUT);
  pinMode(RATE_U, INPUT);

  pinMode(34, OUTPUT); //Power for buttons
  pinMode(40, OUTPUT);
  pinMode(46, OUTPUT);
  pinMode(52, OUTPUT);
  digitalWrite(34, HIGH);
  digitalWrite(40, HIGH);
  digitalWrite(46, HIGH);
  digitalWrite(52, HIGH);
  pinMode(32, OUTPUT); // Ground reference via 10k resistor
  pinMode(38, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(50, OUTPUT);
  digitalWrite(32, LOW);
  digitalWrite(38, LOW);
  digitalWrite(44, LOW);
  digitalWrite(50, LOW);
  
  // VCC and GND for Serial3:
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  digitalWrite(16, LOW);
  digitalWrite(17, HIGH);

  pinMode(13, OUTPUT);

  imu.init();
  
  delay(500);
  
  adxl.init();
  hmc.init();
  itg.init();
  
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(13, LOW);
  
  Serial.begin(9600); // Logging Port
  Serial3.begin(9600); // Debug Port
  
  readConfig();
  
  delay(500);
  
  // Print CSV Header
  Serial.print("\"t\"");
  if( logax ) Serial.print(",\"ax\"");
  if( logay ) Serial.print(",\"ay\"");
  if( logaz ) Serial.print(",\"az\"");
  if( loggx ) Serial.print(",\"gx\"");
  if( loggy ) Serial.print(",\"gy\"");
  if( loggz ) Serial.print(",\"gz\"");
  if( logmx ) Serial.print(",\"mx\"");
  if( logmy ) Serial.print(",\"my\"");
  if( logmz ) Serial.print(",\"mz\"");
  Serial.println("");
  
  Serial3.print("t, frequency, ");
  Serial3.println(frequency);

}








void loop() {
  
  if( toggleState == 1 && digitalRead(RATE_U) == LOW && digitalRead(RATE_D) == LOW ){
    
    then = millis();
    
    Serial.print(millis() - startTime);
    
    if( logax ){
      ax = adxl.getAccel('x');
      Serial.print(",");
      Serial.print(ax);
    }
    if( logay ){
      ay = adxl.getAccel('y');
      Serial.print(",");
      Serial.print(ay);
    }
    if( logaz ){
      az = adxl.getAccel('z');
      Serial.print(",");
      Serial.print(az);
    }
    
    
    if( loggx ){
      gx = itg.getGyro('x');
      Serial.print(",");
      Serial.print(gx);
    }
    if( loggy ){
      gy = itg.getGyro('y');
      Serial.print(",");
      Serial.print(gy);
    }
    if( loggz ){
      gz = itg.getGyro('z');
      Serial.print(",");
      Serial.print(gz);
    }
    
    
    if( logmx ){
      mx = hmc.getMag('x');
      if(mx == -4096){ mx = 0; Serial3.print(millis() - startTime); Serial3.println("mx, error, over/underflow");}
      Serial.print(",");
      Serial.print(mx);
    }
    if( logmy ){
      my = hmc.getMag('y');
      if(my == -4096){ my = 0; Serial3.print(millis() - startTime); Serial3.println("my, error, over/underflow");}
      Serial.print(",");
      Serial.print(my);
    }
    if( logmz ){
      mz = hmc.getMag('z');
      if(mz == -4096){ mz = 0; Serial3.print(millis() - startTime); Serial3.println("mz, error, over/underflow");}
      Serial.print(",");
      Serial.print(mz);
    }
    
    Serial.println("");
    
    digitalWrite(13, notify);
    notify = !notify;
    
    now = millis();
    
    waitTime = rate - (now - then); // Remove program opperation time from delay
    
    if(waitTime < 0){
      waitTime = 0; //If we took longer than rate
      Serial3.println("t, warn, can't keep up");
    }
    
    delay(waitTime);
    
    if( millis() >= logperiod && logfor != 0 ){ // If we've passed the time when we should stop
      toggleState = 0;
    }
    
  }
  
  
  
  if( digitalRead( TOGGLE ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    toggleState = !toggleState;
    if(toggleState == 0) digitalWrite( 13, LOW);
    if(startTime == 0) startTime = millis();
    if(toggleState == 1){
      logperiod = millis() + logfor; // logperiod is the time in millis when we stop
    }
    
    lastBounce = millis();
    
    if( !logax && !logay && !logaz && !loggx && !loggy && !loggz && !logmx && !logmy && !logmz ){
      Serial3.println("l, complaint, IMU.CFG isn't asking for anything to be logged. Pillock.");
      toggleState = 0;
    }
    
  }
  
  
  if( digitalRead( RATE_D ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    digitalWrite(13, HIGH);
    rate = rate + rateInc;
    if( rate >= maxRate){
      rate = maxRate; //max 2 seconds
      delay(250);
      digitalWrite( 13, LOW);
      delay(250);
    }
    lastBounce = millis();
    
    frequency = 1000/rate;
    Serial3.print("t, frequency, ");
    Serial3.print(frequency);
    Serial3.println(" Hz");
    
  }
  
  
  if( digitalRead( RATE_U ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    digitalWrite(13, HIGH);
    rate = rate - rateInc;
    if (rate <= minRate || rate > maxRate){ // If the unsigned long has rocked over
      rate = minRate; // don't allow the rate to increase beyond the defined maximum
      delay(250);
      digitalWrite( 13, LOW);
      delay(250);
    }
    lastBounce = millis();
    
    frequency = 1000/rate;
    Serial3.print("t, frequency, ");
    Serial3.print(frequency);
    Serial3.println(" Hz");
    
  }
  
  
  if( digitalRead( NEWF ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    newFile();
    
  }
  
  // if we're not using the LED for flashing and there are no buttons being pressed, turn it off.
  if( toggleState == 0 && millis() - lastBounce > bounceThreshold){
    digitalWrite( 13, LOW);
  }


}








void readConfig() {
  
  // We need to read the IMU.CFG off the OpenLog
  // Send CTRL+Z 3 times to enter command mode:
  Serial.write(26); // CTRL+Z
  Serial.write(26);
  Serial.write(26);
  delay(10);
  
  // effectively clear Serial port
  Serial.flush();
  while( Serial.available() >= 1 ){
    byte temp = Serial.read();
    if( temp == 26 ){
      Serial3.println("c, error, OpenLog is backchatting; Turn echo off in CONFIG.TXT");
      loop(); //Exit out as Config is unreadable
      break;
    }
  }
  
  delay(10);
  
  // Read AX config:
  Serial.print("read IMU.CFG 0 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logax = true;
      Serial3.println("Logging ax");
    } else if( buffer == '0'){
      logax = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ ax");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read AY config:
  Serial.print("read IMU.CFG 2 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logay = true;
      Serial3.println("Logging ay");
    } else if( buffer == '0'){
      logay = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ ay");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read AZ config:
  Serial.print("read IMU.CFG 4 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logaz = true;
      Serial3.println("Logging az");
    } else if( buffer == '0'){
      logaz = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ az");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read GX config:
  Serial.print("read IMU.CFG 6 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      loggx = true;
      Serial3.println("Logging gx");
    } else if( buffer == '0'){
      loggx = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ gx");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read GY config:
  Serial.print("read IMU.CFG 8 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      loggy = true;
      Serial3.println("Logging gy");
    } else if( buffer == '0'){
      loggy = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ gy");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read GZ config:
  Serial.print("read IMU.CFG 10 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      loggz = true;
      Serial3.println("Logging gz");
    } else if( buffer == '0'){
      loggz = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ gz");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read MX config:
  Serial.print("read IMU.CFG 12 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logmx = true;
      Serial3.println("Logging mx");
    } else if( buffer == '0'){
      logmx = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ mx");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read MY config:
  Serial.print("read IMU.CFG 14 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logmy = true;
      Serial3.println("Logging my");
    } else if( buffer == '0'){
      logmy = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ my");
      loop(); // Config unreadable, return to loop
    }
  }
  
  // Read MZ config:
  Serial.print("read IMU.CFG 16 1");
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer == '1' ){
      logmz = true;
      Serial3.println("Logging mz");
    } else if( buffer == '0'){
      logmz = false;
    } else if( buffer == ','){
      Serial3.println("c, error, Unexpected comma whilst reading IMU.CFG; check formatting @ mz");
      loop(); // Config unreadable, return to loop
    }
  }
  
  
  
  // ri, rma and rmi config will be added eventually
  
  // Clear Serial buffer:
  while( Serial.available() ){
    byte temp = Serial.read();
  }
  
  // Read logfor config:
  int thousands = 0;
  int hundreds = 0;
  int tens = 0;
  int units = 0;
  Serial.print("read IMU.CFG 33 1"); // Thousands
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer < 48 || buffer > 57){
      Serial3.println("c, error, logfor wasn't supplied with a number at the thousands column");
    }
    thousands = buffer - 48; // ASCII '0' has decimal code 48, so numbers are ASCII val - 48
  }
  Serial.print("read IMU.CFG 34 1"); // Hundreds
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer < 48 || buffer > 57){
      Serial3.println("c, error, logfor wasn't supplied with a number at the hundreds column");
    }
    hundreds = buffer - 48;
  }
  Serial.print("read IMU.CFG 35 1"); // Tens
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer < 48 || buffer > 57){
      Serial3.println("c, error, logfor wasn't supplied with a number at the tens column");
    }
    tens = buffer - 48;
  }
  Serial.print("read IMU.CFG 36 1"); // Units
  Serial.write(13);
  Serial.flush();
  delay(5);
  while( Serial.available() ){
    buffer = Serial.read();
    if( buffer < 48 || buffer > 57){
      Serial3.println("c, error, logfor wasn't supplied with a number at the units column");
    }
    units = buffer - 48;
  }
  
  logfor = (thousands * 1000) + (hundreds * 100) + (tens * 10) + units;
  logfor = logfor * 1000; // convert from seconds to millis
  
  // Temporary whilst bug fixing:
  logfor = 20000; // 20 seconds
  
  logperiod = logfor;
  
  Serial3.print("c, notice, will log in bursts of "); Serial3.print(logfor); Serial3.println(" seconds.");
  
  Serial3.println("c, notice, Configuration read from OpenLog");
  
  Serial.print("reset");
  Serial.write(13);
  delay(10);
}







void newFile() {
    
  digitalWrite(13, HIGH);
  Serial.flush();
  delay(250);
  
  Serial.write(26); Serial.write(26); Serial.write(26); //Send openlog command dropin (CTRL+Z)
  delay(10);
  Serial.print("sync\r"); // Ask OpenLog to write any remaining charachters in the buffer to the SD card
  delay(500);
  Serial.print("reset\r");
  delay(1000); // Give the OpenLog some time
  
  
  Serial.print("\"t\"");
  if( logax ) Serial.print(",\"ax\"");
  if( logay ) Serial.print(",\"ay\"");
  if( logaz ) Serial.print(",\"az\"");
  if( loggx ) Serial.print(",\"gx\"");
  if( loggy ) Serial.print(",\"gy\"");
  if( loggz ) Serial.print(",\"gz\"");
  if( logmx ) Serial.print(",\"mx\"");
  if( logmy ) Serial.print(",\"my\"");
  if( logmz ) Serial.print(",\"mz\"");
  Serial.println("");
  
  digitalWrite(13, LOW);
  lastBounce = millis();
  
  toggleState = 0; // reset variables
  rate = minRate;
  startTime = 0;
  notify = 1;
  
}







void makeConfig(){
  
  // Write IMU.CFG to the SD card via OpenLog
  
}
