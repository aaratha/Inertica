#include "audio.hpp"

#define MINIAUDIO_IMPLEMENTATION

#include "miniaudio.h"

void AudioManager::init() {
    if (audioInitialized) return;

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &sineWave;

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


void AudioManager::data_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                                 ma_uint32 frameCount) {
    ma_waveform *pSineWave = (ma_waveform *) pDevice->pUserData;
    ma_waveform_read_pcm_frames(pSineWave, pOutput, frameCount, nullptr);
    (void) pInput;
}

void AudioManager::update(Vec3 latestFiltered) {
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
    ma_waveform_set_frequency(&sineWave, freq);
}
