#include "spark_wiring_i2c.h"
#include "spark_wiring_constants.h"

class BME280{
public:
    int address = 0x76;
    
    String scale="c";
    
    int temp_comp[5];
    int press_comp[11];
    int hum_comp[8];
    
    double tmp_base;
    double temperature;
    double pressure;
    double humidity;
    
    void init();
    void setAddress(int a0);
    
    void getConfigs();
    
    double takeReadings();
    double getTemp(String scale);
};
