#include <Arduino.h>
#include "config.h"
#include "utils/Log.h"
#include "SensorManager.h"
#include "SampleWindow.h"
#include "InferenceEngine.h"
#include "AlertIO.h"
#include "NetworkAlert.h"
#include "Scheduler.h"

static const char* TAG = "MAIN";

static SensorManager   sensor;
static SampleWindow    window;
static InferenceEngine inference;
static AlertIO         alertIO;
static NetworkAlert    network;
static Scheduler       scheduler(sensor, window, inference, alertIO, network);

void setup() {
    Serial.begin(115200);
    delay(500);
    LOGI(TAG, "Fall detector khoi dong");

    alertIO.begin();

    if (!window.begin() || !inference.begin()) {
        LOGE(TAG, "Khoi tao that bai, khoi dong lai");
        alertIO.signalError();
        ESP.restart();
    }
    if (!sensor.begin()) {
        alertIO.signalError();
        ESP.restart();
    }
    if (!scheduler.start()) {
        LOGE(TAG, "Khong tao duoc task");
        alertIO.signalError();
        ESP.restart();
    }
    LOGI(TAG, "He thong dang chay");
}

void loop() {
    // Moi viec chay trong cac FreeRTOS task.
    vTaskDelay(pdMS_TO_TICKS(1000));
}