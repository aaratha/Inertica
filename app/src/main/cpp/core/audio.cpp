#include "audio.hpp"

#define MINIAUDIO_IMPLEMENTATION

#include "miniaudio.h"

// Definition of the static member
Oscillator AudioManager::gOsc;

void AudioManager::init() {
    if (audioInitialized) return;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback = audio_callback;
    deviceConfig.pUserData = &gOsc;

    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        //LOGI("Failed to initialize audio device");
        return;
    }

    ma_waveform_config sineWaveConfig = ma_waveform_config_init(
            device.playback.format,
            device.playback.channels,
            device.sampleRate,
            ma_waveform_type_sine,
            0.2f,    // amplitude
            220.0f   // frequency
    );
    ma_waveform_init(&sineWaveConfig, &sineWave);

    if (ma_device_start(&device) != MA_SUCCESS) {
        //LOGI("Failed to start audio device");
        ma_device_uninit(&device);
        return;
    }

    audioInitialized = true;
    //LOGI("Audio device started!");
}


void AudioManager::audio_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                                  ma_uint32 frameCount) {
    Oscillator *osc = (Oscillator *) pDevice->pUserData;
    float *out = (float *) pOutput;
    float sr = (float) pDevice->sampleRate;

    float freq = osc->prevFreq.load();
    float nextFreq = osc->nextFreq.load();
    float step = (nextFreq - freq) / (float) frameCount;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        freq += step;

        float amp = osc->amp.load();

        float sample = amp * sinf(2.0f * M_PI * osc->phase);
        osc->phase += freq / sr;
        if (osc->phase >= 1.0f) osc->phase -= 1.0f;

        // stereo
        *out++ = sample;
        *out++ = sample;
    }

    osc->prevFreq.store(freq);

    (void) pInput;
}

void AudioManager::updateFrequency(Vec3 latestFiltered) {
    if (!audioInitialized) return;


    float accelX = fabs(latestFiltered.x);
    accelX = ma_clamp(accelX, 0.0f, 5.0f);

    float accelZ = ma_clamp(latestFiltered.z, -5.0f, 5.0f);

    float minAmp = 0.0f, maxAmp = 5.0f;
    float minFreq = 200.f, maxFreq = 1000.f;

    // Amplitude from accelX
    // float amp = (accelX - minAmp) / (maxAmp - minAmp);

    // Frequency from accelZ
    float t = (accelZ - minAmp) / (maxAmp - minAmp);
    float freq = minFreq + t * (maxFreq - minFreq);

    // ma_waveform_set_amplitude(&sineWave, amp);
    // ma_waveform_set_frequency(&sineWave, freq);

    gOsc.nextFreq.store(freq);
}
