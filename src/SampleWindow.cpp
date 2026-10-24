#include "SampleWindow.h"

bool SampleWindow::begin() {
    _mutex = xSemaphoreCreateMutex();
    return _mutex != nullptr;
}

void SampleWindow::push(float ax, float ay, float az) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    float* p = &_buf[_head * AXES];
    p[0] = ax;
    p[1] = ay;
    p[2] = az;
    _head = (_head + 1) % WINDOW_SAMPLES;
    if (_count < WINDOW_SAMPLES) _count++;
    _sinceLast++;
    xSemaphoreGive(_mutex);
}

bool SampleWindow::consumeIfReady(float* out) {
    bool ready = false;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    if (_count >= WINDOW_SAMPLES && _sinceLast >= WINDOW_HOP_SAMPLES) {
        // Khi buffer day, _head chinh la mau cu nhat.
        for (size_t i = 0; i < WINDOW_SAMPLES; i++) {
            const size_t idx = (_head + i) % WINDOW_SAMPLES;
            memcpy(&out[i * AXES], &_buf[idx * AXES], AXES * sizeof(float));
        }
        _sinceLast = 0;
        ready = true;
    }
    xSemaphoreGive(_mutex);
    return ready;
}
