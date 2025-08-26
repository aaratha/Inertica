#include <jni.h>
#include <android/log.h>
#include <string>

#include "core.hpp"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Inertica", __VA_ARGS__)

static SensorManager gSensor;

extern "C" {
JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativeInit(JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    gSensor.init();
}

JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativeUpdate(JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    gSensor.update();
    Vec3 accel = gSensor.getData()[gSensor.getIndex()];
    LOGI("Accel: x=%.3f y=%.3f z=%.3f", accel.x, accel.y, accel.z);
}

JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativePause(JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    gSensor.pause();
}

JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativeResume(JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    gSensor.resume();
}
}