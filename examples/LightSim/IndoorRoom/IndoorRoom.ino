#include <Arduino.h>
#include <LightSim.h>
using namespace SensorSim;

LightSim light(LightScenario::IndoorRoom);

void setup() {
  Serial.begin(115200);
}

void loop() {
  light.update();
  Serial.println(light.read());
  delay(200);
}
