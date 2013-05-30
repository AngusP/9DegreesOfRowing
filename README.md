9DegreesOfRowing
================

9DoF IMU datalogger for a rowing boat or similar application.


The .ino code is an Arduino sketch written for the SparkFun Mega Pro (http://www.sparkfun.com/products/11007) (but should be compatible with any 5v ATMega 2560 board) that uses a SparkFun 9DoF 
(Degrees of Freedom) (http://www.sparkfun.com/products/10724) I2C sensor stick to take acceleration, angular velocity and gyration measurements and then output them to the Serial port at 9600 baud.

Each of the sensors (ADXL accelerometer, HMC magnetometer and ITG gyroscope) are handled individually by library classes, and an additional library handles 
initialising the stick and powering it. To minimise the size of the board, digital pins of the Arduino are used to provide VCC and GND to the stick, as opposed
to soldering to the VCC and GND pins.

The firmware uses four buttons to provide start/stop, increase sample rate, decrease sample rate, and new file (OpenLog compatible) functionality. Again to save
space each momentary switch uses a digital pin for ground, a digital pin as a VCC pullup (through a 10k resistor), and a pin as a digital input. Pin 13's LED is
used to provide simple feedback.

The firmware is designed so that an OpenLog (http://www.sparkfun.com/products/9530) can be plugged directly into the FTDI header and log the CSV output stream, and the new 
file feature is designed specifically for an OpenLog. The Third Serial port (TXO:14 RXI:15 (Arduino pin map, not ATMega)) provides a debug stream again at 9600 baud.



Format for the CSV output:
--------------------------

    t, ax, ay, az, gx, gy, gz, mx, my, mz
Where
    t = time, in milliseconds since logging of this file began. If logging is paused, t still progresses.
    ax, ay, az = accelerometer data, raw with no units or noise reduction
    gx, gy, gz = gyroscope data, raw + noise
    mx, my, mz = magnetometer data, raw + noise

Note that the magnetometer can throw errors, returning a value of -4096 (see datasheet). The firmware replaces these with a zero value, and 
prints to Serial3 "mx, error, over/underflow". This error can be caused by being too close to a strong magnetic field, e.g. Hard Drive, speaker or PSU.

The Serial3 output is in a format similar to CSV, where the metric, issue, and cause are printed separated by commas then followed by CR+LF


OpenLog Config
--------------

For the 9DoR IMU to correctly load it's config from the OpenLog, a couple of things need doing:

In CONFIG.TXT (which is the OpenLog's config file) you need to change echo from 1 (on) to 0 (off) so that when the 9DoR requests data, it doesn't get the command it just issued echoed back to it.
Secondly, a file named IMU.CFG needs to be on the card. If the 9DoR can't get it off the OpenLog, it will create it. It tells the 9DoR which axes on each of the three sensors should be logged,
using a simple CSV format:

    1,1,1,1,1,1,1,1,1,0040,2000,0040

Each of the first 9 values are booleans that successively control ax, ay, az, gx, gy, gz, mx, my, mz. As normal, 1 means "Log this axis" and 0 means "Don't log this axis". The last three values 
set the initial rate (time period between measurements to default to), the maximum rate and the minimum rate, all in milliseconds. Note that all three values must be four digits long and +ve, so
have a maximum range of 0 to 9999 milliseconds, or effectively (based on experimentation) 25 Hz to 0.1 Hz for measuring.




Errors and Notes
----------------

The firmware attempts to follow the frequency for reading as closely as possible, by subtracting the program run time from the time period, but
occasionally (especially close to the maximum sample rate of 25Hz, errors with I2c or exceptions can lead to the program run time exceeding 40 
milliseconds, upon which "t, warn, can't keep up" will be printed to Serial3. The typical variation in the time period is an occasional +1 to +2 milliseconds.




LICENSING - CC BY SA
--------------------

This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/
Attribution: Angus Pearson
Contact: a7cp3ar50n at gmail dot com
