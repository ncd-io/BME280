#include "BME280.h"
#include "Particle.h"

void BME280::init(){
    if(!Wire.isEnabled()) Wire.begin();
    
    String init_msg = "";
    //Humidity sampling rate
    sendCommand(BME280_CTRL_HUM_REGISTER, osrs_h);
    
    // Normal mode, temp and pressure over sampling rate = 1
    sendCommand(BME280_CTRL_MEAS_REGISTER, osrs_p | osrs_t | mode);
    
    // Stand_by time = 1000ms
    sendCommand(BME280_CONFIG_REGISTER, sb_time | filter);
    
    getParams();
}

void BME280::loop(){
    if(loop_delay == 0){
        loop_delay = readRate();
    }
    int now = millis();
    if(now-last_checked > 1000){
        last_checked = now;
        takeReadings();
    }
}

void BME280::getParams(){
    
    int ptdata[25];
    readBytes(0x88, ptdata, 26);
    temp_comp[0] = ptdata[0] + (ptdata[1] << 8);
    temp_comp[1] = ptdata[2] + (ptdata[3] << 8);
    temp_comp[2] = ptdata[4] + (ptdata[5] << 8);
    
    press_comp[0] = ptdata[6] + (ptdata[7] << 8);
    press_comp[1] = ptdata[8] + (ptdata[9] << 8);
    press_comp[2] = ptdata[10] + (ptdata[11] << 8);
    press_comp[3] = ptdata[12] + (ptdata[13] << 8);
    press_comp[4] = ptdata[14] + (ptdata[15] << 8);
    press_comp[5] = ptdata[16] + (ptdata[17] << 8);
    press_comp[6] = ptdata[18] + (ptdata[19] << 8);
    press_comp[7] = ptdata[20] + (ptdata[21] << 8);
    press_comp[8] = ptdata[22] + (ptdata[23] << 8);
    
    hum_comp[0] = ptdata[25];
    
    int humdata[7];
    readBytes(0xE1, humdata, 7);
    hum_comp[1] = humdata[0] + (humdata[1] << 8);
    hum_comp[2] = humdata[2];
    hum_comp[3] = (humdata[3] << 4) + (humdata[4] & 0x0F);
    hum_comp[4] = (humdata[4] >> 4) + (humdata[5] << 4);
    hum_comp[5] = humdata[6];
    char HumComp[100];
    sprintf(HumComp, "[%i,%i,%i,%i,%i,%i,%i]", hum_comp[0], hum_comp[1], hum_comp[2], hum_comp[3], hum_comp[4], hum_comp[5], hum_comp[6]);
    Particle.publish("HumComp", HumComp);
}
// [75,352,0,344,50,30,0]
void BME280::setAddress(int a0){
    address += a0;
}

int BME280::readRate(){
    int rates[6] = {0,1,2,4,8,16};
    
    //assume the max time
    float tmeas = 1.25 + (2.3 * rates[(osrs_t >> 5)]) + (2.3 * rates[(osrs_p & 0x1C) >> 2] + .5) + (2.3 * rates[osrs_h] + 0.575);
    
        if(mode == BME280_MODE_NORMAL){
            switch(sb_time){
                case BME280_T_SB__5:
                    tmeas += .5;
                    break;
                case BME280_T_SB_62_5:
                    tmeas += 62.5;
                    break;
                case BME280_T_SB_125:
                    tmeas += 125;
                    break;
                case BME280_T_SB_250:
                    tmeas += 250;
                    break;
                case BME280_T_SB_500:
                    tmeas += 500;
                    break;
                case BME280_T_SB_1000:
                    tmeas += 1000;
                    break;
                case BME280_T_SB_10:
                    tmeas += 10;
                    break;
                case BME280_T_SB_20:
                    tmeas += 20;
                    break;
            }
        }
        float hz = 1000 / tmeas;
        if(filter){
            switch(filter){
                //Filter settings (filtering out noise)
                case BME280_FILTER_COEF_2:
                    hz = 2000/hz;
                case BME280_FILTER_COEF_4:
                    hz = 5000/hz;
                case BME280_FILTER_COEF_8:
                    hz = 11000/hz;
                case BME280_FILTER_COEF_16:
                    hz = 22000/hz;
            }
        }
        return 1/hz*1000;
}

double BME280::takeReadings(){
    int vardata[8];
    readBytes(BME280_READ_PRESS_MSB, vardata, 8);
    
    int press_var = (vardata[0] << 12) + (vardata[1] << 4) + (vardata[2] >> 4);
    int temp_var = (vardata[3] << 12) + (vardata[4] << 4) + (vardata[5] >> 4);
    int hum_var = (vardata[6] << 8) + vardata[7];
    
    unsigned short t0 = temp_comp[0];
    short t1 = temp_comp[1];
    short t2 = temp_comp[2];
    
    double tvar1 = (((double)temp_var)/16384 - ((double)t0)/1024) * ((double)t1);
    double tvar2 = ((((double)temp_var)/131072 - ((double)t0)/8192) * (((double)temp_var)/131072 - ((double)t0)/8192)) * ((double)t2);
    
    double t_fine = (int)(tvar1+tvar2);
    
    tmp_base = (tvar1+tvar2) / 5120;
    
    temperature = getTemp(scale);
    
    unsigned short p0 = press_comp[0];
    short p1 = press_comp[1];
    short p2 = press_comp[2];
    short p3 = press_comp[3];
    short p4 = press_comp[4];
    short p5 = press_comp[5];
    short p6 = press_comp[6];
    short p7 = press_comp[7];
    short p8 = press_comp[8];
    
    pressure = 0;
    double pvar1 = (t_fine/2) - 64000;
    double pvar2 = pvar1 * pvar1 * ((double)p5) / 32768;
    pvar2 = pvar2 + pvar1 * ((double)p4) * 2;
    pvar2 = (pvar2/4) + (((double)p3) * 65536);
    pvar1 = (((double)p2) * pvar1 * pvar1 / 524288 + ((double)p1) * pvar1) / 524288;
    pvar1 = (1 + pvar1 / 32768) * ((double)p0);
    if(pvar1>0){
        pressure = 1048576 - (double)press_var;
        pressure = (pressure - (pvar2 / 4096)) * 6250 / pvar1;
        pvar1 = ((double)p8) * pressure * pressure / 2147483648;
        pvar2 = pressure * ((double)p7) / 32768;
        pressure = pressure + (pvar1 + pvar2 + ((double)p6)) / 16;
    }
    
    humidity = t_fine - 76800;
    
    char h0 = hum_comp[0];
    short h1 = hum_comp[1];
    char h2 = hum_comp[2];
    short h3 = hum_comp[3];
    short h4 = hum_comp[4];
    signed char h5 = hum_comp[5];
    
    humidity = (hum_var - (((double)h3) * 64 + ((double)h4) / 16384 * humidity)) * (((double)h1) / 65536 * (1 + ((double)h5) / 67108864 * humidity * (1 + ((double)h2) / 67108864 * humidity)));
    
    
    
    humidity = humidity * (1 - ((double)h0) * humidity / 524288);
    
    
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

void BME280::sendCommand(int reg, int cmd){
    Wire.beginTransmission(address);
    Wire.write(reg);
    if(cmd < 256) Wire.write(cmd);
    Wire.endTransmission();
}

void BME280::readBytes(int reg, int *bytes, int length){
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, length);
    for(int i=0;i<length;i++){
        bytes[i] = Wire.read();
    }
}
