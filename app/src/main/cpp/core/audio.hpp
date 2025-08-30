#pragma once

#include <atomic>

#include "utils.hpp"

#include "miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

struct Oscillator {
    std::atomic<float> prevFreq{220.0f};
    std::atomic<float> nextFreq{220.0f};
    std::atomic<float> amp{0.2f};
    float phase = 0.0f;   // not atomic, only used inside callback
};

class AudioManager {
    ma_waveform sineWave;
    ma_device_config deviceConfig;
    ma_device device;

    static Oscillator gOsc;

    bool audioInitialized = false;

public:

    void init();

    static void
    audio_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

    void updateFrequency(Vec3 latestFiltered);
};
