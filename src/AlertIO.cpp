#include "AlertIO.h"

void AlertIO::begin() {
    pinMode(PIN_BUZZER, OUTPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_CANCEL_BTN, INPUT_PULLUP);
    setOutputs(false);
    _lastRaw = _stable = digitalRead(PIN_CANCEL_BTN);
}

void AlertIO::setOutputs(bool on) {
    _outputOn = on;
    digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
    digitalWrite(PIN_LED, on ? HIGH : LOW);
}

void AlertIO::startAlarm() {
    _alarming   = true;
    _lastToggle = millis();
    setOutputs(true);
}

void AlertIO::stopAlarm() {
    _alarming = false;
    setOutputs(false);
}

void AlertIO::update() {
    if (!_alarming) return;
    const uint32_t now = millis();
    if (now - _lastToggle >= ALARM_BLINK_MS) {
        _lastToggle = now;
        setOutputs(!_outputOn);
    }
}

bool AlertIO::cancelPressed() {
    const bool raw = digitalRead(PIN_CANCEL_BTN);
    const uint32_t now = millis();
    if (raw != _lastRaw) {
        _lastRaw = raw;
        _lastChangeMs = now;
    }
    if ((now - _lastChangeMs) >= BUTTON_DEBOUNCE_MS && raw != _stable) {
        _stable = raw;
        if (_stable == LOW) return true;   // canh xuong = vua nhan
    }
    return false;
}

void AlertIO::signalError() {
    for (int i = 0; i < 6; i++) {
        digitalWrite(PIN_LED, i % 2 == 0 ? HIGH : LOW);
        delay(250);
    }
    digitalWrite(PIN_LED, LOW);
}
