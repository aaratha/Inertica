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

void
AudioManager::setAudioState(Oscillator &osc, float amp, float freq, float sampleRate, float *&out) {

    float sample = amp * sinf(2.0f * M_PI * osc.phase);
    osc.phase += freq / sampleRate;
    if (osc.phase >= 1.0f) osc.phase -= 1.0f;

    // stereo
    *out++ = sample;
    *out++ = sample;
}

void AudioManager::audio_callback(ma_device *pDevice, void *pOutput, const void *pInput,
                                  ma_uint32 frameCount) {
    Oscillator *osc = (Oscillator *) pDevice->pUserData;
    float *out = (float *) pOutput;
    float sampleRate = (float) pDevice->sampleRate;


    float amp = osc->prevAmp.load();
    float nextAmp = osc->nextAmp.load();
    float stepAmp = (nextAmp - amp) / (float) frameCount;

    float freq = osc->prevFreq.load();
    float nextFreq = osc->nextFreq.load();
    float stepFreq = (nextFreq - freq) / (float) frameCount;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        amp += stepAmp;
        freq += stepFreq;

        setAudioState(*osc, amp, freq, sampleRate, out);
    }

    osc->prevAmp.store(amp);
    osc->prevFreq.store(freq);

    (void) pInput;
}

void AudioManager::updateFrequency(Vec3 latestFiltered) {
    if (!audioInitialized) return;


    float minAcc = -6.0f, maxAcc = 6.0f;

    float accelX = ma_clamp(latestFiltered.x * 1.3, minAcc, maxAcc);
    float accelZ = ma_clamp(latestFiltered.z, minAcc, maxAcc);

    float minFreq = 100.f, maxFreq = 500.f;
    float minAmp = 0.2f, maxAmp = 1.0f;

    // Amplitude from accelX
    // float amp = (accelX - minAmp) / (maxAmp - minAmp);

    // Frequency from accelZ
    float amp = mapRange(fabs(accelZ), 0, maxAcc, minAmp, maxAmp);
    float freq = mapRange(accelX, minAcc, maxAcc, minFreq, maxFreq);


    gOsc.nextAmp.store(amp);
    gOsc.nextFreq.store(freq);
}
