#pragma once
#include <Arduino.h>

// ============================== Pins ==============================
constexpr int PIN_I2C_SDA     = 21;
constexpr int PIN_I2C_SCL     = 22;
constexpr int PIN_BUZZER      = 25;
constexpr int PIN_LED         = 26;
constexpr int PIN_CANCEL_BTN  = 27;   // INPUT_PULLUP, bam = LOW

// ============================== Sensor ============================
constexpr uint8_t  MPU6050_I2C_ADDR = 0x68;
constexpr uint32_t I2C_CLOCK_HZ     = 400000;

// ============================ Sample window =======================
// Khop voi model Edge Impulse: 25 Hz, 62 mau x 3 truc = 186 float (~2.5s)
constexpr uint32_t SAMPLE_RATE_HZ    = 25;
constexpr uint32_t SAMPLE_PERIOD_MS  = 1000 / SAMPLE_RATE_HZ;                // 40 ms
constexpr size_t   AXES              = 3;                                   // ax, ay, az
constexpr size_t   WINDOW_SAMPLES    = 62;
constexpr size_t   WINDOW_FLOATS     = WINDOW_SAMPLES * AXES;               // 186
constexpr uint32_t WINDOW_HOP_MS     = 500;                                 // suy luan moi 0.5s
constexpr size_t   WINDOW_HOP_SAMPLES = SAMPLE_RATE_HZ * WINDOW_HOP_MS / 1000;

// ============================ Inference ===========================
// 0 = dung thuat toan nguong (stub), 1 = dung model Edge Impulse (lib/ei-fall-detection)
#define USE_EDGE_IMPULSE 1

#define EI_FALL_LABEL        "fall"
constexpr float FALL_SCORE_THRESHOLD = 0.80f;

// Tham so thuat toan nguong (stub), don vi m/s^2
constexpr float STUB_FREEFALL_MS2   = 4.0f;    // ~0.4 g
constexpr float STUB_IMPACT_MS2     = 24.5f;   // ~2.5 g
constexpr size_t STUB_MAX_GAP_SAMPLES = SAMPLE_RATE_HZ / 2;  // va cham phai xay ra trong 0.5s sau roi tu do

// ============================== Alert =============================
constexpr uint32_t ALERT_CANCEL_MS    = 10000;  // thoi gian cho nguoi dung huy
constexpr uint32_t ALERT_COOLDOWN_MS  = 30000;  // khong canh bao lai trong khoang nay
constexpr uint32_t ALARM_BLINK_MS     = 200;
constexpr uint32_t BUTTON_DEBOUNCE_MS = 40;

// ============================== Network ===========================
constexpr uint32_t WIFI_RETRY_MS      = 5000;
constexpr uint8_t  TELEGRAM_RETRIES   = 5;
constexpr uint32_t TELEGRAM_RETRY_MS  = 2000;

// ============================== RTOS ==============================
constexpr uint32_t STACK_SENSOR     = 4096;
constexpr uint32_t STACK_INFERENCE  = 16384;   // EI can nhieu stack hon
constexpr uint32_t STACK_ALERT      = 4096;
constexpr uint32_t STACK_NETWORK    = 10240;   // TLS can nhieu stack
constexpr int CORE_REALTIME = 1;
constexpr int CORE_NETWORK  = 0;