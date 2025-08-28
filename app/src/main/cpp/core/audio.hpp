#pragma once

#include "utils.hpp"

#include "miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

class AudioManager {
  ma_waveform sineWave;
  ma_device_config deviceConfig;
  ma_device device;

    bool audioInitialized = false;

public:

  void init();
  static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
  void update(Vec3 latestFiltered);
};
