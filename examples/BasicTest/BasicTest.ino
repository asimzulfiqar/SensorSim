#include <DHTSim.h>
using namespace SensorSim;

DHTSim dht(DHTScenario::IndoorRoom);

void setup() {
  Serial.begin(115200);
}

void loop() {
  dht.update();

  Serial.print("Temperature: ");
  Serial.print(dht.read());
  Serial.print(" °C   Humidity: ");
  Serial.println(dht.readHumidity());

  delay(1000);
}