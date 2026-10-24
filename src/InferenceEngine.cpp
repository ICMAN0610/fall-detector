#include "InferenceEngine.h"
#include <math.h>
#include "utils/Log.h"

#if USE_EDGE_IMPULSE
#include <fall-detector_inferencing.h>   // doi ten neu project EI cua ban khac
static_assert(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE == WINDOW_FLOATS,
              "Cau hinh cua so khong khop voi model Edge Impulse");
#endif

static const char* TAG = "INFER";

#if USE_EDGE_IMPULSE
static const float* s_window = nullptr;
static int eiGetData(size_t offset, size_t length, float* out) {
    memcpy(out, s_window + offset, length * sizeof(float));
    return 0;
}
#endif

bool InferenceEngine::begin() {
#if USE_EDGE_IMPULSE
    LOGI(TAG, "Edge Impulse: %d nhan, %d float/cua so",
         EI_CLASSIFIER_LABEL_COUNT, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
#else
    LOGW(TAG, "Dung thuat toan nguong (stub), chua dung model Edge Impulse");
#endif
    return true;
}

bool InferenceEngine::classify(const float* window, InferenceResult& out) {
#if USE_EDGE_IMPULSE
    s_window = window;
    signal_t signal;
    signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    signal.get_data = &eiGetData;

    ei_impulse_result_t result;
    if (run_classifier(&signal, &result, false) != EI_IMPULSE_OK) {
        LOGE(TAG, "run_classifier that bai");
        return false;
    }
    out.score = 0.0f;
    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (strcmp(result.classification[i].label, EI_FALL_LABEL) == 0) {
            out.score = result.classification[i].value;
        }
    }
#else
    // Mau hinh te nga: roi tu do (|a| nho) ngay sau do la va cham manh (|a| lon).
    int  lastFreefall = -1;
    bool fall = false;
    float maxMag = 0.0f;
    for (size_t i = 0; i < WINDOW_SAMPLES; i++) {
        const float x = window[i * AXES], y = window[i * AXES + 1], z = window[i * AXES + 2];
        const float mag = sqrtf(x * x + y * y + z * z);
        if (mag > maxMag) maxMag = mag;
        if (mag < STUB_FREEFALL_MS2) {
            lastFreefall = (int)i;
        } else if (mag > STUB_IMPACT_MS2 && lastFreefall >= 0 &&
                   (i - (size_t)lastFreefall) <= STUB_MAX_GAP_SAMPLES) {
            fall = true;
        }
    }
    out.score = fall ? 1.0f : 0.0f;
    (void)maxMag;
#endif
    out.isFall = out.score >= FALL_SCORE_THRESHOLD;
    return true;
}
