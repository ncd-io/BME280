#include "spark_wiring_i2c.h"
#include "spark_wiring_constants.h"

//Oversampling of humidity data

#define BME280_CTRL_HUM_REGISTER 0xF2
#define BME280_OSRS_H_0 0x00
#define BME280_OSRS_H_1 0x01
#define BME280_OSRS_H_2 0x02
#define BME280_OSRS_H_4 0x03
#define BME280_OSRS_H_8 0x04
#define BME280_OSRS_H_16 0x05

//Status register

#define BME280_STATUS_REGISTER 0XF3
//masks
#define BME280_CONVERSION_RUNNING 0X04
#define BME280_NVP_COPYING 0X01

//Control Measure Register

#define BME280_CTRL_MEAS_REGISTER 0XF4
//Oversampling of temperature data
#define BME280_OSRS_T_0 0x00
#define BME280_OSRS_T_1 0x20
#define BME280_OSRS_T_2 0x40
#define BME280_OSRS_T_4 0x60
#define BME280_OSRS_T_8 0x80
#define BME280_OSRS_T_16 0xA0
//Oversampling of pressure data
#define BME280_OSRS_P_0 0x00
#define BME280_OSRS_P_1 0x04
#define BME280_OSRS_P_2 0x08
#define BME280_OSRS_P_4 0x0C
#define BME280_OSRS_P_8 0x10
#define BME280_OSRS_P_16 0x14
//Settings mode
#define BME280_MODE_SLEEP 0x00
#define BME280_MODE_FORCED 0x01
#define BME280_MODE_NORMAL 0x03

//Config register This should be written to in sleep mode

#define BME280_CONFIG_REGISTER 0xF5
//Standby time for normal mode
#define BME280_T_SB__5 0x00
#define BME280_T_SB_62_5 0x20
#define BME280_T_SB_125 0x40
#define BME280_T_SB_250 0x60
#define BME280_T_SB_500 0x80
#define BME280_T_SB_1000 0xA0
#define BME280_T_SB_10 0xC0
#define BME280_T_SB_20 0xE0
//Filter settings (filtering out noise)
#define BME280_FILTER_COEF_0 0x00
#define BME280_FILTER_COEF_2 0x04
#define BME280_FILTER_COEF_4 0x08
#define BME280_FILTER_COEF_8 0x0C
#define BME280_FILTER_COEF_16 0x10

//Read registers
#define BME280_READ_PRESS_MSB 0xF7
#define BME280_READ_PRESS_LSB 0xF8
#define BME280_READ_PRESS_XLSB 0xF9
#define BME280_READ_HUM_MSB 0xFA
#define BME280_READ_HUM_LSB 0xFB
#define BME280_READ_HUM_XLSB 0xFC
#define BME280_READ_TEMP_MSB 0xFD
#define BME280_READ_TEMP_LSB 0xFE

class BME280{
public:
    int address = 0x76;
    
    String scale="c";
    
    int osrs_h = BME280_OSRS_H_1;
    int osrs_p = BME280_OSRS_P_1;
    int osrs_t = BME280_OSRS_T_1;
    
    int mode = BME280_MODE_NORMAL;
    
    int sb_time = BME280_T_SB_1000;
    int filter = BME280_FILTER_COEF_0;
    
    int temp_comp[5];
    int press_comp[11];
    int hum_comp[8];
    
    double tmp_base;
    double temperature;
    double pressure;
    double humidity;
    
    void init();
    void setAddress(int a0);
    
    void getParams();
    
    int readRate();
    
    double takeReadings();
    double getTemp(String scale);
    
    void sendCommand(int reg, int cmd);
    void readBytes(int reg, int *bytes, int length);
    
    void loop();
    
    int last_checked=0;
    int loop_delay=0;
    
    
};
