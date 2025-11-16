#pragma once
#include <Arduino.h>
#include "SensorSim.h"

namespace SensorSim {

enum class LightScenario {
    Manual,
    OutdoorSunny,
    OutdoorCloudy,
    IndoorRoom,
    Greenhouse
};

class LightSim : public BaseSensor {
public:

    // -------- FULL MANUAL MODE --------
    LightSim(
        float luxMin, float luxMax,
        float driftPerSec = 5.0f,
        float noiseStdPerSec = 1.0f,
        float dailyAmp = 0.0f,
        float dailyPhase = 0.0f,
        float timeScale = 0.02f   // 1 full simulated day ≈ 2 minutes
    )
        : scenario(LightScenario::Manual),
          luxMin(luxMin), luxMax(luxMax),
          driftPerSec(driftPerSec),
          noiseStd(noiseStdPerSec),
          dailyAmp(dailyAmp),
          dailyPhase(dailyPhase),
          timeScale(timeScale)
    {
        lux = randomFloat(luxMin, luxMax);
        lastUpdate = millis();
    }


    // -------- SCENARIO MODE --------
    LightSim(LightScenario scenario)
        : scenario(scenario)
    {
        configureScenario();
        lastUpdate = millis();
    }


    // -------- UPDATE --------
    void update() override {

        unsigned long now = millis();
        float dt = (now - lastUpdate) / 1000.0f;
        if (dt <= 0) return;
        lastUpdate = now;

        simulatedHour += dt * timeScale;
        if (simulatedHour >= 24.0f) simulatedHour -= 24.0f;

        switch (scenario) {

            case LightScenario::OutdoorSunny:
                updateOutdoorSunny(simulatedHour, dt);
                break;

            case LightScenario::OutdoorCloudy:
                updateOutdoorCloudy(simulatedHour, dt);
                break;

            case LightScenario::IndoorRoom:
                updateIndoorRoom(dt);
                break;

            case LightScenario::Greenhouse:
                updateGreenhouse(simulatedHour, dt);
                break;

            case LightScenario::Manual:
                updateManual(simulatedHour, dt);
                break;
        }
    }

    float read() override { return lux; }

private:

    // ------------------------
    // INTERNAL STATE
    // ------------------------
    LightScenario scenario;

    float lux = 0.0f;

    float simulatedHour = 0.0f;
    unsigned long lastUpdate = 0;

    // Manual configs
    float luxMin = 0, luxMax = 1000;
    float driftPerSec = 5.0f;
    float noiseStd = 1.0f;

    float dailyAmp = 0.0f;
    float dailyPhase = 0.0f;

    float timeScale = 0.02f;


    // ------------------------
    // UTILITIES
    // ------------------------
    float randomFloat(float a, float b) {
        return a + (b - a) * ((float)random(0, 10000) / 10000.0f);
    }

    float gaussianNoise(float std) {
        float u1 = randomFloat(0.0001f, 1.0f);
        float u2 = randomFloat(0.0001f, 1.0f);
        return std * sqrtf(-2.0f * logf(u1)) * cosf(2 * PI * u2);
    }

    float clamp(float x, float a, float b) {
        return (x < a) ? a : (x > b) ? b : x;
    }

    float boundedRandomWalkDT(float current, float minV, float maxV,
                              float driftPerSec, float dt) {

        float step = driftPerSec * dt;
        current += randomFloat(-step, step);

        if (current < minV) current = minV;
        if (current > maxV) current = maxV;

        return current;
    }

    // ------------------------
    // PHYSICALLY ACCURATE DAYLIGHT CURVE
    // ------------------------
    float daylightCurve(float hour, float peakLux) {
        // Convert hour (0–24) into smooth daylight:
        // sunrise ~6, noon ~12, sunset ~18
        float angle = 2.0f * PI * (hour / 24.0f) - (PI / 2.0f);

        float val = (sinf(angle) + 1.0f) * 0.5f;  // 0 → 1 → 0

        return peakLux * val;
    }

    // ------------------------
    // SCENARIO CONFIG
    // ------------------------
    void configureScenario() {

        switch (scenario) {

            case LightScenario::OutdoorSunny:
                luxMin = 0;
                luxMax = 100000;
                timeScale = 0.02f;
                lux = 2000;
                break;

            case LightScenario::OutdoorCloudy:
                luxMin = 0;
                luxMax = 25000;
                timeScale = 0.02f;
                lux = 1500;
                break;

            case LightScenario::IndoorRoom:
                luxMin = 50;
                luxMax = 150;
                timeScale = 0.0f;
                lux = 90;
                break;

            case LightScenario::Greenhouse:
                luxMin = 2000;
                luxMax = 30000;
                timeScale = 0.015f;
                lux = 12000;
                break;

            default:
                break;
        }
    }

    // ------------------------
    // MANUAL UPDATE
    // ------------------------
    void updateManual(float hours, float dt) {

        lux += randomFloat(-driftPerSec * dt, driftPerSec * dt);
        lux += gaussianNoise(noiseStd * dt);

        if (dailyAmp > 0.0f) {
            float daily = dailyAmp *
                          sinf(2 * PI * (hours / 24.0f + dailyPhase));
            lux += daily * dt;
        }

        lux = clamp(lux, luxMin, luxMax);
    }


    // ------------------------
    // OUTDOOR SUNNY — sunrise → noon → sunset (smooth)
    // ------------------------
    void updateOutdoorSunny(float hours, float dt) {

        float peakLux = 100000.0f;

        float daily = daylightCurve(hours, peakLux);

        // Tiny atmospheric variation
        float noise = gaussianNoise(100.0f * dt);

        lux = daily + noise;

        if (lux < 0) lux = 0;
        if (lux > peakLux) lux = peakLux;
    }


    // ------------------------
    // OUTDOOR CLOUDY — dimmer, smooth variability
    // ------------------------
    void updateOutdoorCloudy(float hours, float dt) {

        float peakLux = 25000.0f;

        float daily = daylightCurve(hours, peakLux);

        // Cloud factor 0.4–0.8
        float cloudFactor = randomFloat(0.4f, 0.8f);

        // Slow-changing cloud noise
        float cloudNoise = gaussianNoise(200.0f * dt);

        lux = daily * cloudFactor + cloudNoise;

        if (lux < 0) lux = 0;
        if (lux > peakLux) lux = peakLux;
    }


    // ------------------------
    // INDOOR ROOM — stable, tiny drift
    // ------------------------
    void updateIndoorRoom(float dt) {
        lux = boundedRandomWalkDT(lux, luxMin, luxMax, 3.0f, dt);
    }

    // ------------------------
    // GREENHOUSE — amplified outdoor light
    // ------------------------
    void updateGreenhouse(float hours, float dt) {

        float peakLux = 30000;

        float daily = daylightCurve(hours, peakLux);

        // plant-shading + humidity noise
        float noise = gaussianNoise(150.0f * dt);

        lux = daily + noise;

        lux = clamp(lux, luxMin, luxMax);
    }

};

} // namespace SensorSim
