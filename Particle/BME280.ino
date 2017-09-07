// This #include statement was automatically added by the Particle IDE.
#include "BME280.h"

BME280 sensor;

void setup() {
    sensor.scale = "f";
    sensor.init();
    Particle.variable("temperature", sensor.temperature);
    Particle.variable("humidity", sensor.humidity);
    Particle.variable("pressure", sensor.pressure);
}
int last_checked = 0;
void loop() {
    int now = millis();
    if(now-last_checked > 1000){
        last_checked = now;
        sensor.takeReadings();
    }
}
