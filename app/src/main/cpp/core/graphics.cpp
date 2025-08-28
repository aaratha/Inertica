#include "graphics.hpp"

void GraphicsManager::init(AAssetManager *assetManager) {
    AAsset *vertexShaderAsset =
            AAssetManager_open(assetManager, "shader.glslv", AASSET_MODE_BUFFER);
    assert(vertexShaderAsset != nullptr);
    const void *vertexShaderBuf = AAsset_getBuffer(vertexShaderAsset);
    assert(vertexShaderBuf != NULL);
    off_t vertexShaderLength = AAsset_getLength(vertexShaderAsset);
    vertexShaderSource =
            std::string((const char *) vertexShaderBuf, (size_t) vertexShaderLength);
    AAsset_close(vertexShaderAsset);

    AAsset *fragmentShaderAsset =
            AAssetManager_open(assetManager, "shader.glslf", AASSET_MODE_BUFFER);
    assert(fragmentShaderAsset != NULL);
    const void *fragmentShaderBuf = AAsset_getBuffer(fragmentShaderAsset);
    assert(fragmentShaderBuf != NULL);
    off_t fragmentShaderLength = AAsset_getLength(fragmentShaderAsset);
    fragmentShaderSource = std::string((const char *) fragmentShaderBuf,
                                       (size_t) fragmentShaderLength);
    AAsset_close(fragmentShaderAsset);

    generateXPos();
}

void GraphicsManager::surfaceCreated() {


    shaderProgram = createProgram(vertexShaderSource, fragmentShaderSource);
    assert(shaderProgram != 0);
    GLint getPositionLocationResult =
            glGetAttribLocation(shaderProgram, "vPosition");
    assert(getPositionLocationResult != -1);
    vPositionHandle = (GLuint) getPositionLocationResult;
    GLint getSensorValueLocationResult =
            glGetAttribLocation(shaderProgram, "vSensorValue");
    assert(getSensorValueLocationResult != -1);
    vSensorValueHandle = (GLuint) getSensorValueLocationResult;
    GLint getFragColorLocationResult =
            glGetUniformLocation(shaderProgram, "uFragColor");
    assert(getFragColorLocationResult != -1);
    uFragColorHandle = (GLuint) getFragColorLocationResult;
}

void GraphicsManager::surfaceChanged(int w, int h) { glViewport(0, 0, w, h); }

void GraphicsManager::generateXPos() {
    for (auto i = 0; i < SENSOR_HISTORY_LENGTH; i++) {
        float t =
                static_cast<float>(i) / static_cast<float>(SENSOR_HISTORY_LENGTH - 1);
        xPos[i] = -1.f * (1.f - t) + 1.f * t;
    }
}


GLuint GraphicsManager::createProgram(const std::string &pVertexSource,
                                      const std::string &pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    GLuint program = glCreateProgram();
    assert(program != 0);
    glAttachShader(program, vertexShader);
    glAttachShader(program, pixelShader);
    glLinkProgram(program);
    GLint programLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &programLinked);
    assert(programLinked != 0);
    glDeleteShader(vertexShader);
    glDeleteShader(pixelShader);
    return program;
}

GLuint GraphicsManager::loadShader(GLenum shaderType, const std::string &pSource) {
    GLuint shader = glCreateShader(shaderType);
    assert(shader != 0);
    const char *sourceBuf = pSource.c_str();
    glShaderSource(shader, 1, &sourceBuf, NULL);
    glCompileShader(shader);
    GLint shaderCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    assert(shaderCompiled != 0);
    return shader;
}


void GraphicsManager::render(const Vec3 *sensorData, int sensorDataIndex) {
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glEnableVertexAttribArray(vPositionHandle);
    glVertexAttribPointer(vPositionHandle, 1, GL_FLOAT, GL_FALSE, 0, xPos);

    glEnableVertexAttribArray(vSensorValueHandle);

    // --- Accelerometer: X, Y, Z ---
    glVertexAttribPointer(vSensorValueHandle, 1, GL_FLOAT, GL_FALSE,
                          sizeof(Vec3),
                          &sensorData[sensorDataIndex].x);
    glUniform4f(uFragColorHandle, 1.0f, 1.0f, 0.0f, 1.0f); // yellow
    glDrawArrays(GL_LINE_STRIP, 0, SENSOR_HISTORY_LENGTH);

    glVertexAttribPointer(vSensorValueHandle, 1, GL_FLOAT, GL_FALSE,
                          sizeof(Vec3),
                          &sensorData[sensorDataIndex].y);
    glUniform4f(uFragColorHandle, 1.0f, 0.0f, 1.0f, 1.0f); // magenta
    glDrawArrays(GL_LINE_STRIP, 0, SENSOR_HISTORY_LENGTH);

    glVertexAttribPointer(vSensorValueHandle, 1, GL_FLOAT, GL_FALSE,
                          sizeof(Vec3),
                          &sensorData[sensorDataIndex].z);
    glUniform4f(uFragColorHandle, 0.0f, 1.0f, 1.0f, 1.0f); // cyan
    glDrawArrays(GL_LINE_STRIP, 0, SENSOR_HISTORY_LENGTH);
}
