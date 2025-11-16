#pragma once

namespace SensorSim {

class BaseSensor {
public:
    virtual void update() = 0;
    virtual float read() = 0;
    virtual ~BaseSensor() {}
};

} // namespace SensorSim
