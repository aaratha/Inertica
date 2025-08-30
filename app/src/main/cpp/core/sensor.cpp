#include "sensor.hpp"

#include <cassert>

ASensorManager *SensorManager::AcquireASensorManagerInstance(void) {
    typedef ASensorManager *(*PF_GETINSTANCEFORPACKAGE)(const char *name);
    void *androidHandle = dlopen("libandroid.so", RTLD_NOW);
    PF_GETINSTANCEFORPACKAGE getInstanceForPackageFunc =
            (PF_GETINSTANCEFORPACKAGE) dlsym(androidHandle,
                                             "ASensorManager_getInstanceForPackage");
    if (getInstanceForPackageFunc) {
        return getInstanceForPackageFunc(kPackageName);
    }

    typedef ASensorManager *(*PF_GETINSTANCE)();
    PF_GETINSTANCE getInstanceFunc =
            (PF_GETINSTANCE) dlsym(androidHandle, "ASensorManager_getInstance");
    // by all means at this point, ASensorManager_getInstance should be available
    assert(getInstanceFunc);
    return getInstanceFunc();
}

void SensorManager::init(ALooper *looper) {
    manager = AcquireASensorManagerInstance();
    assert(manager != NULL);
    accelerometer = ASensorManager_getDefaultSensor(manager,
                                                    ASENSOR_TYPE_LINEAR_ACCELERATION);
    assert(accelerometer != NULL);
    // looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    assert(looper != NULL);
    eventQueue = ASensorManager_createEventQueue(
            manager, looper, LOOPER_ID_USER, NULL, NULL);
    assert(eventQueue != NULL);
    auto status =
            ASensorEventQueue_enableSensor(eventQueue, accelerometer);
    assert(status >= 0);
    status = ASensorEventQueue_setEventRate(
            eventQueue, accelerometer, SENSOR_REFRESH_PERIOD_US);
    assert(status >= 0);
    (void) status;  // to silent unused compiler warning
}

void SensorManager::update() {
    //ALooper_pollOnce(0, NULL, NULL, NULL);
    ASensorEvent event;
    float a = SENSOR_FILTER_ALPHA;
    while (ASensorEventQueue_getEvents(eventQueue, &event, 1) > 0) {
        sensorDataFilter.x =
                a * event.acceleration.x + (1.0f - a) * sensorDataFilter.x;
        sensorDataFilter.y =
                a * event.acceleration.y + (1.0f - a) * sensorDataFilter.y;
        sensorDataFilter.z =
                a * event.acceleration.z + (1.0f - a) * sensorDataFilter.z;
    }
    latestFiltered = sensorDataFilter;

    sensorData[sensorDataIndex] = sensorDataFilter;
    sensorData[SENSOR_HISTORY_LENGTH + sensorDataIndex] = sensorDataFilter;
    sensorDataIndex = (sensorDataIndex + 1) % SENSOR_HISTORY_LENGTH;
}

void SensorManager::pause() {
    ASensorEventQueue_disableSensor(eventQueue, accelerometer);
}

void SensorManager::resume() {
    if (!eventQueue || !accelerometer) {
        __android_log_print(ANDROID_LOG_ERROR, "sensor",
                            "resume() called before init(): queue=%p accel=%p", eventQueue,
                            accelerometer);
        return;
    }
    int status = ASensorEventQueue_enableSensor(eventQueue, accelerometer);
    if (status < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "sensor",
                            "enableSensor failed: %d", status);
    }
    status = ASensorEventQueue_setEventRate(eventQueue, accelerometer, SENSOR_REFRESH_PERIOD_US);
    if (status < 0) {
        __android_log_print(ANDROID_LOG_ERROR, "sensor",
                            "setEventRate failed: %d", status);
    }
}

const Vec3 *SensorManager::getData() {
    return sensorData;
}

const int SensorManager::getIndex() {
    return sensorDataIndex;
}