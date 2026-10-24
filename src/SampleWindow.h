#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

// Bo dem vong luu WINDOW_SAMPLES mau (ax, ay, az xen ke), an toan giua cac task.
class SampleWindow {
public:
    bool begin();
    void push(float ax, float ay, float az);

    // Neu cua so da day va co it nhat WINDOW_HOP_SAMPLES mau moi, copy cua so
    // (theo thu tu thoi gian, cu -> moi) vao `out` (WINDOW_FLOATS float) va tra true.
    bool consumeIfReady(float* out);

private:
    float  _buf[WINDOW_FLOATS] = {0};
    size_t _head      = 0;   // vi tri se ghi tiep theo
    size_t _count     = 0;
    size_t _sinceLast = 0;
    SemaphoreHandle_t _mutex = nullptr;
};