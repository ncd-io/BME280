#include "BME280.h"
#include "Particle.h"

void BME280::init(){
    Wire.begin();
    
    //Humidity sampling rate
    Wire.beginTransmission(address);
    Wire.write(0xF2);
    Wire.write(0x01);
    Wire.endTransmission();
    
    // Normal mode, temp and pressure over sampling rate = 1
    Wire.beginTransmission(address);
    Wire.write(0xF4);
    Wire.write(0x27);
    Wire.endTransmission();
    
    // Stand_by time = 1000ms
    Wire.beginTransmission(address);
    Wire.write(0xF5);
    Wire.write(0xA0);
    Wire.endTransmission();
    
    getConfigs();
}

void BME280::getConfigs(){
    Wire.beginTransmission(address);
    Wire.write(0x88);
    Wire.endTransmission();
    Wire.requestFrom(address, 25);
    
    for(int i=0;i<13;i++){
        if(i < 3){
            temp_comp[i] = Wire.read() + (Wire.read() << 8);
        }else if(i < 12){
            press_comp[i-3] = Wire.read() + (Wire.read() << 8);
        }else{
            hum_comp[0] = Wire.read();
        }
    }
    
    Wire.beginTransmission(address);
    Wire.write(0xE1);
    Wire.endTransmission();
    Wire.requestFrom(address, 8);
    
    for(int i=1;i<6;i++){
        switch(i){
            case 1:
                hum_comp[i] = Wire.read() + (Wire.read() << 8);
                break;
            case 3:
                hum_comp[i] = (Wire.read() << 4) + (Wire.read() & 0xF);
                break;
            case 4:
                hum_comp[i] = (Wire.read() >> 4) + (Wire.read() << 4);
                break;
            default:
                hum_comp[i] = Wire.read();
                break;
        }
    }
}

void BME280::setAddress(int a0){
    address += a0;
}

double BME280::takeReadings(){
    Wire.beginTransmission(address);
    Wire.write(0xF7);
    Wire.endTransmission();
    Wire.requestFrom(address, 8);
    
    long press_var = (Wire.read() << 12) + (Wire.read() << 4) + (Wire.read() >> 4);
    long temp_var = (Wire.read() << 12) + (Wire.read() << 4) + (Wire.read() >> 4);
    int hum_var = (Wire.read() << 8) + Wire.read();
    
    double tvar1 = (((double)temp_var)/16384 - ((double)temp_comp[0])/1024) * ((double)temp_comp[1]);
    double tvar2 = ((((double)temp_var)/131072 - ((double)temp_comp[0])/8192) * (((double)temp_var)/131072 - ((double)temp_comp[0])/8192)) * ((double)temp_comp[2]);
    
    double t_fine = (int)(tvar1+tvar2);
    
    tmp_base = (tvar1+tvar2) / 5120;
    
    temperature = getTemp(scale);
    
    pressure = 0;
    double pvar1 = (t_fine/2) - 64000;
    double pvar2 = pvar1 * pvar1 * ((double)press_comp[5]) / 32768;
    pvar2 = pvar2 + pvar1 * ((double)press_comp[4]) * 2;
    pvar2 = (pvar2/4) + (((double)press_comp[3]) * 65536);
    pvar1 = (((double)press_comp[2]) * pvar1 * pvar1 / 524288 + ((double)press_comp[1]) * pvar1) / 524288;
    pvar1 = (1 + pvar1 / 32768) * ((double)press_comp[0]);
    if(pvar1>0){
        pressure = 1048576 - (double)press_var;
        pressure = (pressure - (pvar2 / 4096)) * 6250 / pvar1;
        pvar1 = ((double)press_comp[8]) * pressure * pressure / 2147483648;
        pvar2 = pressure * ((double)press_comp[7]) / 32768;
        pressure = pressure + (pvar1 + pvar2 + ((double)press_comp[6])) / 16;
    }
    
    humidity = t_fine - 76800;
    humidity = (hum_var - (((double)hum_comp[3]) * 64 + ((double)hum_comp[4]) / 16384 * humidity)) * (((double)hum_comp[1]) / 65536 * (1 + ((double)hum_comp[5]) / 67108864 * humidity * (1 + ((double)hum_comp[2]) / 67108864 * humidity)));
    humidity = humidity * (1 - ((double)hum_comp[0]) * humidity / 524288);
    
    if(humidity > 100) humidity = 100;
    else if(humidity < 0) humidity = 0;
}


double BME280::getTemp(String scalea){
    if(scalea.equalsIgnoreCase("c")){
        return tmp_base;
    }
    if(scalea.equalsIgnoreCase("k")){
        return tmp_base + 273.15;
    }
    if(scalea.equalsIgnoreCase("f")){
        return tmp_base * 1.8 + 32;
    }
}
