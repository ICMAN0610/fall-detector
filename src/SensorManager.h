#pragma once
#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include "config.h"

// Doc gia toc MPU6050 (don vi m/s^2).
class SensorManager {
public:
    bool begin();
    bool read(float& ax, float& ay, float& az);

private:
    Adafruit_MPU6050 _mpu;
    bool _ok = false;
};
