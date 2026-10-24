#include "SensorManager.h"
#include <Wire.h>
#include "utils/Log.h"

static const char* TAG = "SENSOR";

bool SensorManager::begin() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    Wire.setClock(I2C_CLOCK_HZ);

    if (!_mpu.begin(MPU6050_I2C_ADDR, &Wire)) {
        LOGE(TAG, "Khong tim thay MPU6050 (SDA=%d, SCL=%d)", PIN_I2C_SDA, PIN_I2C_SCL);
        return false;
    }
    // 16G de khong bi bao hoa khi va cham manh.
    _mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    _mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    _ok = true;
    LOGI(TAG, "MPU6050 san sang");
    return true;
}

bool SensorManager::read(float& ax, float& ay, float& az) {
    if (!_ok) return false;
    sensors_event_t a, g, t;
    if (!_mpu.getEvent(&a, &g, &t)) return false;
    ax = a.acceleration.x;
    ay = a.acceleration.y;
    az = a.acceleration.z;
    return true;
}
