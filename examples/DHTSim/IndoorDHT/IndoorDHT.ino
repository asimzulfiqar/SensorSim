#include <Arduino.h>
#include <DHTSim.h>

using namespace SensorSim;

DHTSim dht(DHTScenario::IndoorRoom);

void setup() {
  Serial.begin(115200);
}

void loop() {
  dht.update();
  Serial.print("Indoor Temp: ");
  Serial.print(dht.read(), 2);
  Serial.print(" °C   Humidity: ");
  Serial.println(dht.readHumidity(), 2);
  delay(1000);
}
