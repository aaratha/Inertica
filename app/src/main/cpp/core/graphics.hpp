#pragma once

#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <string>
#include <cassert>

#include "utils.hpp"

class GraphicsManager {
  std::string vertexShaderSource;
  std::string fragmentShaderSource;

  GLuint shaderProgram;
  GLuint vPositionHandle;
  GLuint vSensorValueHandle;
  GLuint uFragColorHandle;
  GLfloat xPos[SENSOR_HISTORY_LENGTH];

public:
  void init(AAssetManager *assetManager);
  void surfaceCreated();
  GLuint createProgram(const std::string &pVertexSource,
                       const std::string &pFragmentSource);
  GLuint loadShader(GLenum shaderType, const std::string &pSource);
  void surfaceChanged(int w, int h);
  void generateXPos();
  void render(const Vec3* sensorData, int sensorDataIndex);
};
