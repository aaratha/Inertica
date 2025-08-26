#include <jni.h>
#include <android/log.h>
#include <string>

#include "core.hpp"


static SensorManager gSensor;
static GraphicsManager gGraphics;

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Inertica", __VA_ARGS__)

extern "C" {
JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativeInit(JNIEnv *env, jobject type, jobject assetManager) {
    (void) type;
    AAssetManager *nativeAssetManager = AAssetManager_fromJava(env, assetManager);
    ALooper* mainLooper = ALooper_forThread(); // must be called on main thread
    if (!mainLooper) {
        ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
        mainLooper = ALooper_forThread();
    }
    gSensor.init(mainLooper);
    gGraphics.init(nativeAssetManager);
}

JNIEXPORT void JNICALL 
Java_com_aaratha_inertica_MainActivity_nativeSurfaceCreated(
        JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    LOGI("GL_VERSION: %s", glGetString(GL_VERSION));
    LOGI("GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("GL_EXTENSIONS: %s", glGetString(GL_EXTENSIONS));
    gGraphics.surfaceCreated();
}

JNIEXPORT void JNICALL Java_com_aaratha_inertica_MainActivity_nativeSurfaceChanged(
        JNIEnv *env, jobject type, jint width, jint height) {
    (void) env;
    (void) type;
    gGraphics.surfaceChanged(width, height);
}

JNIEXPORT void JNICALL
Java_com_aaratha_inertica_MainActivity_nativeDrawFrame(JNIEnv *env, jobject type) {
    (void) env;
    (void) type;
    gSensor.update();
    const Vec3* sensorData = gSensor.getData();
    int sensorDataIndex = gSensor.getIndex();
    gGraphics.render(sensorData, sensorDataIndex);
    // LOGI("Accel: x=%.3f y=%.3f z=%.3f", accel.x, accel.y, accel.z);
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
