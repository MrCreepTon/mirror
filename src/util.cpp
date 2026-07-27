#include "util.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CScene* GetGlobalScene() {
    return &(*(CScene*)GLOBAL_SCENE_ADDR);
}

CCamera* GetGlobalCamera() {
    return &(*(CCamera*)GLOBAL_CAMERA_ADDR);
}

RwDevice* GetOpenDevice() {
    char* rwEngineInstance = *(char**)RW_ENGINE_INSTANCE_ADDR;
    return &(*(RwDevice*)(rwEngineInstance + RW_ENGINE_OPEN_DEVICE_OFFSET));
}

bool IsGameLoaded() {
    int gameState = *(int*)GAME_STATE;
    return (gameState == GAME_STATE_LOADED);
}

void SetMatrixRotation(CMatrix* matrix, float rotX, float rotY, float rotZ) {
    float radX = (rotX + 90.0f) * (float)M_PI / 180.0f;
    float radY = rotY * (float)M_PI / 180.0f;
    float radZ = (rotZ + 180.0f) * (float)M_PI / 180.0f;

    float sX = std::sin(radX);
    float cX = std::cos(radX);
    float sY = std::sin(radY);
    float cY = std::cos(radY);
    float sZ = std::sin(radZ);
    float cZ = std::cos(radZ);

    matrix->right.x = cZ * cY - sZ * sX * sY;
    matrix->right.y = sZ * cY + cZ * sX * sY;
    matrix->right.z = -cX * sY;

    matrix->up.x = cZ * sY + sZ * sX * cY;
    matrix->up.y = sZ * sY - cZ * sX * cY;
    matrix->up.z = cX * cY;

    matrix->at.x = -sZ * cX;
    matrix->at.y = cZ * cX;
    matrix->at.z = sX;
}