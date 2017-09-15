# About

This Library is intended for use with any BME280 GPIO board available from ncd.io

###Developer information
NCD has been designing and manufacturing computer control products since 1995.  We have specialized in hardware design and manufacturing of Relay controllers for 20 years.  We pride ourselves as being the industry leader of computer control relay products.  Our products are proven reliable and we are very excited to support Particle.  For more information on NCD please visit ncd.io

###Requirements
- The Python SMBus Module: https://pypi.python.org/pypi/smbus-cffi/
- An I2C connection to an BME280 Board from ncd.io
- Knowledge base for developing and programming with Python.

### Version
1.0.0

### How to use this library

The libary must be imported into your application. Create an SMBus object that to utilize the I2C bus. Create a BME280 object and pass it the SMBus object to start to communicate to the chip. You can optionally pass in kwargs to the object at instantiation that will let you calibrate the device to better suit your application.

It is recommended that you read the datasheet for the BME280 to better understand these settings and how you should calibrate them. These calibration are optional and the default values have been shown to be accurate under normal conditions by our instrumentation.

###Public accessible methods
```cpp
get_readings(temperature_unit)
```
>This function returns the temperature, humidity, and pressure in a keyed set.

>The temperature_unit passed into the method is a string of either c, f, or k to represent how you would like the temperature 
>to be returned.


