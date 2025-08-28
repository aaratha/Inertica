#pragma once

#include "utils.hpp"

#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <android/sensor.h>
#include <dlfcn.h>
#include <vector>


class SensorManager {
    ASensorManager *manager;
    const ASensor *accelerometer;
    ASensorEventQueue *eventQueue;

    Vec3 sensorData[SENSOR_HISTORY_LENGTH * 2];
    Vec3 sensorDataFilter;
    int sensorDataIndex;

public:
    Vec3 latestFiltered;

    ASensorManager *AcquireASensorManagerInstance(void);

    void init(ALooper *looper);

    void update();

    void pause();

    void resume();

    const Vec3 *getData();

    const int getIndex();
};

