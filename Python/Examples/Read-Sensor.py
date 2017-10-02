#import smbus for i2c communications
import smbus
import time
#import the chip library
import bme280

# Get I2C bus, this is I2C Bus 1
bus = smbus.SMBus(1)
#kwargs is a Python set that contains the address of your device as well as desired range and bandwidth
#refer to the chip's datasheet to determine what values you should use to suit your project
#The default address for this chip is 0x76, this simply allows you to manually set it for multi-board chains.
kwargs = {'address': 0x76, 'humidity_sampling_rate': 0x01, 'pressure_sampling_rate': 0x04, 'temperature_sampling_rate': 0x20, 'mode': 0x03, 'standby_time': 0xA0, 'filter': 0x00}
#create the BME280 object from the BME280 library and pass it the kwargs and com bus.
#the object requires that you pass it the bus object so that it can communicate and share the bus with other chips/boards if necessary
bme280 = bme280.BME280(bus, kwargs)

while True :
    #print out the readings.
    #the readings will be return in a set keyed as pressure, temperature, and humidity for the corresponding values
    #read the temperature back in fahrenheit
    print bme280.get_readings('f')
    #read the temperature back in kelvin
    print bme280.get_readings('k')
    #read the temperature back in celsius (this is also the default value so () would work
    print bme280.get_readings('c')
    #this sleep is not required
    time.sleep(.25)
