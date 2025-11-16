#include <Arduino.h>
#include <LightSim.h>
using namespace SensorSim;

LightSim light(LightScenario::OutdoorSunny);

void setup() {
  Serial.begin(115200);
}

void loop() {
  light.update();
  Serial.println(light.read());  // Serial Plotter friendly
  delay(200);
}
