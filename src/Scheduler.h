#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "config.h"
#include "utils/Log.h"
#include "SensorManager.h"
#include "SampleWindow.h"
#include "InferenceEngine.h"
#include "AlertIO.h"
#include "NetworkAlert.h"

// Tao va chay cac FreeRTOS task:
//   sensorTask    (core 1): doc MPU6050 @SAMPLE_RATE_HZ -> SampleWindow
//   inferenceTask (core 1): lay cua so, suy luan, day su kien nga
//   alertTask     (core 1): coi/LED 10s cho nut huy; neu khong huy -> gui Telegram
//   networkTask   (core 0): duy tri WiFi va gui tin Telegram
class Scheduler {
public:
    Scheduler(SensorManager& sensor, SampleWindow& window, InferenceEngine& inference,
              AlertIO& alert, NetworkAlert& net)
        : _sensor(sensor), _window(window), _inference(inference), _alert(alert), _net(net) {}

    bool start() {
        _fallQueue = xQueueCreate(2, sizeof(float));
        _netQueue  = xQueueCreate(4, sizeof(float));
        if (!_fallQueue || !_netQueue) return false;

        bool ok = true;
        ok &= xTaskCreatePinnedToCore(sensorTask,    "sensor",    STACK_SENSOR,    this, 4, nullptr, CORE_REALTIME) == pdPASS;
        ok &= xTaskCreatePinnedToCore(inferenceTask, "inference", STACK_INFERENCE, this, 3, nullptr, CORE_REALTIME) == pdPASS;
        ok &= xTaskCreatePinnedToCore(alertTask,     "alert",     STACK_ALERT,     this, 2, nullptr, CORE_REALTIME) == pdPASS;
        ok &= xTaskCreatePinnedToCore(networkTask,   "network",   STACK_NETWORK,   this, 1, nullptr, CORE_NETWORK)  == pdPASS;
        return ok;
    }

private:
    static void sensorTask(void* arg) {
        auto* s = static_cast<Scheduler*>(arg);
        TickType_t last = xTaskGetTickCount();
        for (;;) {
            float x, y, z;
            if (s->_sensor.read(x, y, z)) s->_window.push(x, y, z);
            vTaskDelayUntil(&last, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
        }
    }

    static void inferenceTask(void* arg) {
        auto* s = static_cast<Scheduler*>(arg);
        static float buf[WINDOW_FLOATS];
        for (;;) {
            if (s->_window.consumeIfReady(buf)) {
                InferenceResult r;
                const bool cooledDown = !s->_hasAlerted ||
                                        (millis() - s->_lastAlertEndMs) >= ALERT_COOLDOWN_MS;
                if (s->_inference.classify(buf, r) && r.isFall && !s->_alerting && cooledDown) {
                    s->_alerting = true;
                    LOGW("SCHED", "Phat hien nga! score=%.2f", r.score);
                    xQueueSend(s->_fallQueue, &r.score, 0);
                }
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }

    static void alertTask(void* arg) {
        auto* s = static_cast<Scheduler*>(arg);
        float score;
        for (;;) {
            if (xQueueReceive(s->_fallQueue, &score, portMAX_DELAY) != pdTRUE) continue;

            s->_alert.startAlarm();
            const uint32_t t0 = millis();
            bool cancelled = false;
            while (millis() - t0 < ALERT_CANCEL_MS) {
                s->_alert.update();
                if (s->_alert.cancelPressed()) { cancelled = true; break; }
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            s->_alert.stopAlarm();

            if (cancelled) {
                LOGI("SCHED", "Nguoi dung da huy canh bao");
            } else {
                LOGW("SCHED", "Het %lus khong huy -> gui Telegram", (unsigned long)(ALERT_CANCEL_MS / 1000));
                xQueueSend(s->_netQueue, &score, 0);
            }
            s->_lastAlertEndMs = millis();
            s->_hasAlerted = true;
            s->_alerting = false;
        }
    }

    static void networkTask(void* arg) {
        auto* s = static_cast<Scheduler*>(arg);
        s->_net.begin();
        float score;
        for (;;) {
            s->_net.maintain();
            if (xQueueReceive(s->_netQueue, &score, pdMS_TO_TICKS(500)) == pdTRUE) {
                for (uint8_t i = 0; i < TELEGRAM_RETRIES; i++) {
                    s->_net.maintain();
                    if (s->_net.sendFallAlert(score)) break;
                    vTaskDelay(pdMS_TO_TICKS(TELEGRAM_RETRY_MS));
                }
            }
        }
    }

    SensorManager&   _sensor;
    SampleWindow&    _window;
    InferenceEngine& _inference;
    AlertIO&         _alert;
    NetworkAlert&    _net;

    QueueHandle_t _fallQueue = nullptr;
    QueueHandle_t _netQueue  = nullptr;
    volatile bool     _alerting       = false;
    volatile bool     _hasAlerted     = false;
    volatile uint32_t _lastAlertEndMs = 0;
};