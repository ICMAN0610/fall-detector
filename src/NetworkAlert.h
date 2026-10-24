#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"

// Ket noi WiFi va gui canh bao qua Telegram Bot.
class NetworkAlert {
public:
    NetworkAlert();

    void begin();                       // bat dau ket noi WiFi (khong chan)
    void maintain();                    // goi dinh ky: tu ket noi lai WiFi, dong bo gio (NTP)
    bool isReady() const;               // WiFi OK + da dong bo gio
    bool sendFallAlert(float score);

private:
    WiFiClientSecure  _client;          // phai khai bao truoc _bot
    UniversalTelegramBot _bot;
    uint32_t _lastTryMs  = 0;
    bool     _timeSynced = false;
    bool     _ntpStarted = false;
};