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
void loop() {
    sensor.loop();
}
