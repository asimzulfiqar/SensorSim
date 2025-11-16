# SensorSim — Virtual Sensor Simulation Library for Arduino & ESP32

SensorSim is a lightweight, extensible simulation library for **Arduino**, **ESP32**, and **PlatformIO**, designed to generate **realistic virtual sensor data** when real hardware is not available, unreliable, or not yet purchased.

It is ideal for:
- IoT prototyping  
- Teaching & demos  
- Testing dashboards (Grafana, MQTT, InfluxDB, Home Assistant)  
- Offline development  
- Algorithm design without hardware dependency  

---

## ✨ Features

- Multiple sensor simulators (growing list)
- Realistic environmental behavior
- Smooth time-based transitions using `millis()`
- Manual mode → user-defined ranges, drift, noise
- Scenario mode → indoor, outdoor, AC office, greenhouse, etc.
- Hardware-agnostic: works on **any** microcontroller
- Designed for Arduino IDE & PlatformIO
- Fully object-oriented and extensible
- Stable random walks & daily cycles

---

## 📦 Current Sensors Implemented

### **1. DHTSim**
Simulates **temperature & humidity** with:
- Manual mode  
- Indoor room  
- Greenhouse  
- AC office  

Includes:
- Correct dt-scaled drift  
- Gaussian noise  
- Daily cycles (for scenarios)  
- Smooth transitions

### **2. LightSim**
Simulates **ambient light intensity (lux)** for:
- Outdoor Sunny  
- Outdoor Cloudy  
- Indoor Room  
- Greenhouse  
- Manual mode  

Includes:
- Physically accurate daylight model (smooth sunrise → noon → sunset)
- Cloud attenuation  
- Indoor stability mode  
- Fully configurable manual mode  

> ⚠️ **Note:** LightSim is still evolving.  
> A dedicated TODO is included below.

---

## 🚀 Installation

### **Arduino IDE**
1. Download this repository or clone it  
2. Place the folder in:  
   ```
   Documents/Arduino/libraries/SensorSim
   ```
3. Restart Arduino IDE

### **PlatformIO**
Add to `platformio.ini`:
```ini
lib_deps = 
    SensorSim
```
(or use `lib_deps` with Git URL once public)

---

## 🧪 Basic Usage Example (DHTSim Scenario)

```cpp
#include <Arduino.h>
#include <DHTSim.h>

using namespace SensorSim;

DHTSim dht(DHTScenario::IndoorRoom);

void setup() {
  Serial.begin(115200);
}

void loop() {
  dht.update();
  Serial.print("Temp: ");
  Serial.print(dht.read());
  Serial.print(" °C   Humidity: ");
  Serial.println(dht.readHumidity());
  delay(1000);
}
```

---

## 💡 LightSim Example (Outdoor Sunny + Serial Plotter)

```cpp
#include <Arduino.h>
#include <LightSim.h>

using namespace SensorSim;

LightSim light(LightScenario::OutdoorSunny);

void setup() {
  Serial.begin(115200);
}

void loop() {
  light.update();
  Serial.println(light.read());
  delay(200);
}
```

Open **Tools → Serial Plotter** to see sunrise → noon → sunset curve.

---

## ⚙️ Manual Mode Example (DHTSim)

```cpp
DHTSim dht(
    20, 30,   // temperature range
    40, 60,   // humidity range
    0.05,     // drift per second
    0.02      // noise std
);
```

---

# 🗺️ TODO / Roadmap

### **Core Improvements**
- [ ] Improve LightSim realism (better smoothing + cloud layers)
- [ ] Add persistent pseudo-random seeds for deterministic runs
- [ ] Create separate `examples/` folder for each sensor

### **New Sensors (Planned)**
- [ ] `PIRSim` — human motion/occupancy
- [ ] `SoilMoistureSim`
- [ ] `CO2Sim` / GasSim
- [ ] `IMUSim` (accelerometer/gyro)
- [ ] `PressureSim` (BMP280-style)
- [ ] `MultiSensor` packs (environment bundles)
- [ ] Battery simulation (voltage decay curves)

### **Developer Tools**
- [ ] Add unit tests (Unity/Catch2)
- [ ] Add Arduino Library Manager JSON
- [ ] Publish to PlatformIO registry

---

## 🏗️ Project Structure

```
SensorSim/
│
├── src/
│   ├── SensorSim.h       (base class)
│   ├── DHTSim.h
│   ├── LightSim.h
│   └── ... future sensors
│
├── examples/
│   ├── DHTSim_Basic/
│   ├── DHTSim_Indoor/
│   └── LightSim_Sunny/
│
└── README.md
```

---

## 🤝 Contributing

Pull requests are welcome!

If you'd like to contribute:
1. File an issue  
2. Describe the sensor or improvement  
3. Submit a PR with clean formatting  

All new sensors must:
- use `BaseSensor::update()` pattern  
- support manual + scenario modes  
- include at least 1 example  

---

## 📜 License

MIT License  
You are free to use this in commercial and open-source projects.

---

## 📌 Version

**SensorSim v0.0.1 — Initial Public Release**

---
