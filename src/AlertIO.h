#pragma once
#include <Arduino.h>
#include "config.h"

// Dieu khien coi, LED va nut huy canh bao (chi GPIO).
class AlertIO {
public:
    void begin();

    void startAlarm();
    void stopAlarm();
    void update();            // goi dinh ky khi dang bao dong de nhap nhay coi/LED

    bool cancelPressed();     // true 1 lan khi nut duoc nhan (da khu doi)
    void signalError();       // nhap nhay LED bao loi (blocking, ~3s)

private:
    void setOutputs(bool on);

    bool     _alarming   = false;
    bool     _outputOn   = false;
    uint32_t _lastToggle = 0;

    bool     _lastRaw       = HIGH;
    bool     _stable        = HIGH;
    uint32_t _lastChangeMs  = 0;
};
