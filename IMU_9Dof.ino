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

int minRate = 40;
unsigned int rate = minRate; // Delay for loop
int frequency = 1000/rate; // 100ms in 1 second

boolean notify = 0; //Notification LED state
boolean toggleState = 0;
boolean rateUState = 0;
boolean rateDState = 0;
unsigned int rateInc = 10; // milliseconds to increase rate by each time
char header[] = "\"t\",\"ax\",\"ay\",\"az\",\"gx\",\"gy\",\"gz\",\"mx\",\"my\",\"mz\"";

unsigned long lastBounce = 0;  //debouncing control
int bounceThreshold = 250; // 1/4 of a second

// More stable timing between readings:
long then = 0;
long now = 0;
long waitTime = 0;

int mx, my, mz;
int ax, ay, az;
int gx, gy, gz;


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

  pinMode(13, OUTPUT);

  imu.init();

  Serial.begin(9600); // Logging Port
  Serial2.begin(9600); // Debug Port

  delay(1000);

  adxl.init();
  hmc.init();
  itg.init();

  delay(1000);
  Serial.println(header);
  Serial2.print("Recording at ");
  Serial2.println(frequency);

}



void loop() {
  
  if( toggleState == 1 && digitalRead(RATE_U) == LOW && digitalRead(RATE_D) == LOW ){
    
    then = millis();
    
    ax = adxl.getAccel('x');
    ay = adxl.getAccel('y');
    az = adxl.getAccel('z');

    gx = itg.getGyro('x');
    gy = itg.getGyro('y');
    gz = itg.getGyro('z');

    mx = hmc.getMag('x');
    my = hmc.getMag('y');
    mz = hmc.getMag('z');
    
    // Error Management:
    if(mx == -4096){ mx = 0; Serial2.print(millis() - startTime); Serial2.println("mx, error, over/underflow");}
    if(my == -4096){ my = 0; Serial2.print(millis() - startTime); Serial2.println("my, error, over/underflow");}
    if(mz == -4096){ mz = 0; Serial2.print(millis() - startTime); Serial2.println("mz, error, over/underflow");}
    
    Serial.print(millis()-startTime);
    Serial.print(",");
    
    Serial.print(ax); 
    Serial.print(",");
    Serial.print(ay); 
    Serial.print(",");
    Serial.print(az); 
    Serial.print(",");

    Serial.print(gx); 
    Serial.print(",");
    Serial.print(gy); 
    Serial.print(",");
    Serial.print(gz); 
    Serial.print(",");

    Serial.print(mx); 
    Serial.print(",");
    Serial.print(my); 
    Serial.print(",");
    Serial.println(mz);
    
    digitalWrite(13, notify);
    notify = !notify;
    
    now = millis();
    
    waitTime = rate - (now - then); // Remove program opperation time from delay
    
    if(waitTime < 0){
      waitTime = 0; //If we took longer than rate
      Serial2.println("t, warn, can't keep up");
    }
    
    delay(waitTime);
    
  }  
  // Need to tie inputs to ground
  if( digitalRead( TOGGLE ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    toggleState = !toggleState;
    if(toggleState == 0) digitalWrite( 13, LOW);
    if( startTime == 0) startTime = millis();
    lastBounce = millis();
    
  }
  
  if( digitalRead( RATE_D ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    digitalWrite(13, HIGH);
    rate = rate + rateInc;
    if( rate >= 1000){
      rate = 1000; //max 1 second
      delay(250);
      digitalWrite( 13, LOW);
      delay(250);
    }
    lastBounce = millis();
    
  }
  
  if( digitalRead( RATE_U ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    digitalWrite(13, HIGH);
    rate = rate - rateInc;
    if (rate <= minRate || rate > 1000){ // If the unsigned long has rocked over
      rate = minRate; // Instructions take time
      delay(250);
      digitalWrite( 13, LOW);
      delay(250);
    }
    lastBounce = millis();
    
  }
  
  if( digitalRead( NEWF ) == HIGH && millis() - lastBounce > bounceThreshold){
    
    digitalWrite(13, HIGH);
    Serial.flush();
    delay(250);
    
    Serial.write(0x1A); Serial.write(0x1A); Serial.write(0x1A); //Send openlog command dropin
    Serial.print("reset\r");
    delay(1000);
    
    Serial.println(header);
    digitalWrite(13, LOW);
    lastBounce = millis();
    
    toggleState = 0; // reset variables
    rate = minRate;
    startTime = 0;
    notify = 1;
    
  }
  
  if( toggleState == 0 && millis() - lastBounce > bounceThreshold){
    digitalWrite( 13, LOW);
  }
  
  //Serial.print(" Rate: "); Serial.println(rate); 


}

