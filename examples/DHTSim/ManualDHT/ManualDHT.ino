#include <Arduino.h>
#include <DHTSim.h>

using namespace SensorSim;

// Simple manual configuration
DHTSim dht(
    20, 30,      // temp range
    40, 60,      // humidity range
    0.05, 0.08,  // drift per second
    0.01, 0.02,  // noise per second
    0.0, 0.0,    // no daily cycle
    0.0,         // daily cycle phase
    0.0001       // time scale
);

// DHTSim dht(
//     22, 28,
//     40, 65,
//     0.03, 0.05,     // drift
//     0.01, 0.015,    // noise
//     1.5, 5.0,        // daily amplitude (°C, %)
//     0.1,             // daily phase shift
//     0.00015          // time scale
// );

// // Constant environment + tiny noise only
// DHTSim dht(
//     22, 22,        // temp fixed at 22°C
//     50, 50,        // humidity fixed at 50%
//     0.0, 0.0,      // no drift
//     0.02, 0.03,    // noise only
//     0.0, 0.0,      // no daily cycle
//     0.0,           // daily phase
//     0.0001
// );

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
