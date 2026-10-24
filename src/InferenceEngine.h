#pragma once
#include <Arduino.h>
#include "config.h"

struct InferenceResult {
    float score  = 0.0f;    // xac suat te nga [0..1]
    bool  isFall = false;
};

// Suy luan te nga. USE_EDGE_IMPULSE=0 -> thuat toan nguong (stub);
// USE_EDGE_IMPULSE=1 -> model Edge Impulse trong lib/ei-fall-detection.
class InferenceEngine {
public:
    bool begin();
    // `window`: WINDOW_FLOATS float (ax,ay,az xen ke, m/s^2), cu -> moi.
    bool classify(const float* window, InferenceResult& out);
};
