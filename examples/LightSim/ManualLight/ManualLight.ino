#include <Arduino.h>
#include <LightSim.h>
using namespace SensorSim;

LightSim light(
  100, 200,       // min-max lux
  5.0,            // drift
  1.0,            // noise
  20.0,           // daily amp
  0.2,            // daily phase
  0.0002          // time scale
);

void setup() {
  Serial.begin(115200);
}

void loop() {
  light.update();
  Serial.println(light.read());
  delay(200);
}
