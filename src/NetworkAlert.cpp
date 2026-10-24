#include "NetworkAlert.h"
#include <time.h>
#include "secrets.h"
#include "utils/Log.h"

static const char* TAG = "NET";

NetworkAlert::NetworkAlert() : _bot(TELEGRAM_BOT_TOKEN, _client) {}

void NetworkAlert::begin() {
    _client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _lastTryMs = millis();
    LOGI(TAG, "Dang ket noi WiFi '%s'...", WIFI_SSID);
}

void NetworkAlert::maintain() {
    if (WiFi.status() != WL_CONNECTED) {
        _timeSynced = false;
        if (millis() - _lastTryMs >= WIFI_RETRY_MS) {
            _lastTryMs = millis();
            LOGW(TAG, "WiFi mat ket noi, thu lai...");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        }
        return;
    }

    if (!_timeSynced) {
        // TLS can dong ho dung de kiem tra chung chi.
        if (!_ntpStarted) {
            configTime(0, 0, "pool.ntp.org", "time.google.com");   // chi goi 1 lan, SNTP tu chay nen
            _ntpStarted = true;
        }
        if (time(nullptr) < 1700000000) {
            if (millis() - _lastTryMs >= 1000) {
                _lastTryMs = millis();
                LOGI(TAG, "Cho dong bo gio NTP...");
            }
        } else {
            _timeSynced = true;
            LOGI(TAG, "WiFi OK (IP %s), gio da dong bo", WiFi.localIP().toString().c_str());
        }
    }
}

bool NetworkAlert::isReady() const {
    return WiFi.status() == WL_CONNECTED && _timeSynced;
}

bool NetworkAlert::sendFallAlert(float score) {
    if (!isReady()) {
        LOGW(TAG, "Mang chua san sang, khong gui duoc");
        return false;
    }
    char msg[160];
    snprintf(msg, sizeof(msg),
             "🚨 PHÁT HIỆN TÉ NGÃ!\nĐộ tin cậy: %.0f%%\nThiết bị hoạt động: %lu giây",
             score * 100.0f, (unsigned long)(millis() / 1000));
    const bool ok = _bot.sendMessage(TELEGRAM_CHAT_ID, msg, "");
    LOGI(TAG, "Gui Telegram: %s", ok ? "OK" : "THAT BAI");
    return ok;
}
