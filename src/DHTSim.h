#pragma once
#include <Arduino.h>
#include "SensorSim.h"

namespace SensorSim {

enum class DHTScenario {
    Manual,
    IndoorRoom,
    Greenhouse,
    OfficeAC
};

class DHTSim : public BaseSensor {
public:

// FULL manual constructor with complete configurability
DHTSim(
    float tempMin, float tempMax,
    float humMin,  float humMax,

    // random walk drift per second
    float tempStepPerSec = 0.05f,
    float humStepPerSec  = 0.10f,

    // gaussian noise std dev per second
    float tempNoiseStdPerSec = 0.01f,
    float humNoiseStdPerSec  = 0.02f,

    // daily cycle parameters (set amplitude=0 to disable)
    float tempDailyAmp = 0.0f,
    float humDailyAmp  = 0.0f,
    float dailyPhase   = 0.0f, // shift in cycles (0–1), 0 = noon

    // time scale
    float timeScale = 0.0001f
)
    : scenario(DHTScenario::Manual),

      tMin(tempMin), tMax(tempMax),
      hMin(humMin), hMax(humMax),

      tempStep(tempStepPerSec),
      humStep(humStepPerSec),

      tempNoiseStd(tempNoiseStdPerSec),
      humNoiseStd(humNoiseStdPerSec),

      tempDailyAmp(tempDailyAmp),
      humDailyAmp(humDailyAmp),
      dailyPhase(dailyPhase),

      timeScale(timeScale)
{
    temperature = randomFloat(tMin, tMax);
    humidity    = randomFloat(hMin, hMax);
    lastUpdate = millis();
}


    // Scenario constructor
    DHTSim(DHTScenario scenario)
        : scenario(scenario)
    {
        configureScenario();
        lastUpdate = millis();
    }

    void update() override {

        unsigned long now = millis();
        float dt = (now - lastUpdate) / 1000.0f;  // seconds since last update
        if (dt <= 0) return;
        lastUpdate = now;

        // advance environmental clock (used for daily cycles)
        simulatedHour += dt * timeScale;
        if (simulatedHour >= 24.0f) simulatedHour -= 24.0f;

        switch (scenario) {

            case DHTScenario::IndoorRoom:
                updateIndoor(simulatedHour, dt);
                break;

            case DHTScenario::Greenhouse:
                updateGreenhouse(simulatedHour, dt);
                break;

            case DHTScenario::OfficeAC:
                updateOfficeAC(simulatedHour, dt);
                break;

            case DHTScenario::Manual:
                updateManual(dt);
                break;
        }
    }

    float read() override { return temperature; }
    float readHumidity() { return humidity; }

private:
    // Scenario mode
    DHTScenario scenario;

    // State
    float temperature = 25.0f;
    float humidity = 50.0f;

    float simulatedHour = 0.0f;
    unsigned long lastUpdate = 0;

    float timeScale = 0.0001f;

    // Manual mode configuration (FULLY CONFIGURABLE)
    float tMin = 20, tMax = 30;
    float hMin = 40, hMax = 60;

    float tempStep = 0.05f;     // drift/sec
    float humStep  = 0.10f;

    float tempNoiseStd = 0.01f; // noise/sec
    float humNoiseStd  = 0.02f;

    float tempDailyAmp = 0.0f;  // °C daily amplitude
    float humDailyAmp  = 0.0f;  // % daily amplitude

    float dailyPhase = 0.0f;    // daily cycle phase shift

    float maxStep = 0.3;        // legacy (safe to keep)
    float noiseStd = 0.2;

    // =========================================================
    // Utility functions
    // =========================================================
    float randomFloat(float a, float b) {
        return a + (b - a) * ((float)random(0, 10000) / 10000.0f);
    }

    float gaussianNoise(float std) {
        float u1 = randomFloat(0.0001f, 1.0f);
        float u2 = randomFloat(0.0001f, 1.0f);
        return std * sqrt(-2.0f * log(u1)) * cos(2 * PI * u2);
    }

    float clamp(float x, float a, float b) {
        return (x < a) ? a : (x > b) ? b : x;
    }

    // dt-aware bounded random walk
    float boundedRandomWalkDT(float current, float minV, float maxV,
                              float stepSize, float dt) {

        float step = stepSize * dt;
        current += randomFloat(-step, step);

        if (current < minV) current = minV;
        if (current > maxV) current = maxV;

        return current;
    }

    // =========================================================
    // Scenario initialization
    // =========================================================
    void configureScenario() {

        switch (scenario) {
            case DHTScenario::IndoorRoom:
                temperature = randomFloat(22.4f, 23.1f);
                humidity    = randomFloat(47.0f, 52.0f);
                break;

            case DHTScenario::Greenhouse:
                temperature = randomFloat(27.0f, 32.0f);
                humidity    = randomFloat(75.0f, 95.0f);
                break;

            case DHTScenario::OfficeAC:
                temperature = randomFloat(21.0f, 24.0f);
                humidity    = randomFloat(35.0f, 45.0f);
                break;

            default:
                break;
        }
    }

    // =========================================================
    // Scenario-specific update logic
    // =========================================================

    // Manual mode: dt-aware noise + drift
    void updateManual(float dt) {

    // 1. Random walk drift (time-based)
    float stepT = tempStep * dt;
    float stepH = humStep  * dt;

    temperature += randomFloat(-stepT, stepT);
    humidity    += randomFloat(-stepH, stepH);

    // 2. Gaussian noise (time-based)
    temperature += gaussianNoise(tempNoiseStd * dt);
    humidity    += gaussianNoise(humNoiseStd * dt);

    // 3. Optional daily cycle
    if (tempDailyAmp > 0.0f) {
        float cycle = sin(2 * PI * (simulatedHour / 24.0f + dailyPhase));
        temperature += tempDailyAmp * cycle * dt;  
    }

    if (humDailyAmp > 0.0f) {
        float cycle = sin(2 * PI * (simulatedHour / 24.0f + dailyPhase));
        humidity += humDailyAmp * cycle * dt;
    }

    // 4. Bound to user-defined limits
    temperature = clamp(temperature, tMin, tMax);
    humidity    = clamp(humidity,    hMin, hMax);
}


    // Indoor stable environment
    void updateIndoor(float hours, float dt) {

        const float tempMin = 22.4f, tempMax = 23.1f;
        const float humMin  = 47.0f, humMax  = 52.0f;

        temperature = boundedRandomWalkDT(temperature, tempMin, tempMax, 0.02f, dt);
        humidity    = boundedRandomWalkDT(humidity,    humMin,  humMax, 0.05f, dt);
    }

    // Greenhouse with mild daily cycles
    void updateGreenhouse(float hours, float dt) {

        float dayFactor = sin(2 * PI * (hours / 24.0f)); // -1 to 1

        float tMin = 27.0f + 0.4f * dayFactor;
        float tMax = 32.0f + 0.4f * dayFactor;

        float hMin = 75.0f + 1.0f * dayFactor;
        float hMax = 95.0f + 1.0f * dayFactor;

        temperature = boundedRandomWalkDT(temperature, tMin, tMax, 0.05f, dt);
        humidity    = boundedRandomWalkDT(humidity,    hMin, hMax,  0.10f, dt);
    }

    // Office AC dynamics (cooling cycles)
    void updateOfficeAC(float hours, float dt) {

        bool acOn = (hours >= 9 && hours < 18);

        float tMin = acOn ? 21.2f : 22.8f;
        float tMax = acOn ? 22.0f : 23.2f;

        float hMin = acOn ? 37.0f : 42.0f;
        float hMax = acOn ? 41.0f : 47.0f;

        temperature = boundedRandomWalkDT(temperature, tMin, tMax, 0.03f, dt);
        humidity    = boundedRandomWalkDT(humidity,    hMin, hMax,  0.08f, dt);
    }
};

} // namespace SensorSim
