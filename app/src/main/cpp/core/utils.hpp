#pragma once

#include <cstdint>
#include <GLES2/gl2.h>

constexpr char *kPackageName = "com.aaratha.inertica";

constexpr int LOOPER_ID_USER = 3;
constexpr int SENSOR_HISTORY_LENGTH = 100;
constexpr int SENSOR_REFRESH_RATE_HZ = 100;
constexpr int32_t SENSOR_REFRESH_PERIOD_US = int32_t(1000000 / SENSOR_REFRESH_RATE_HZ);
constexpr float SENSOR_FILTER_ALPHA = 0.1f;

struct Vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
};
