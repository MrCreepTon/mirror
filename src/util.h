#pragma once

#include "game_structures.h"

#define GAME_STATE 0xC8D4C0
#define GAME_STATE_LOADED 9

#define GLOBAL_SCENE_ADDR 0xC17038
#define GLOBAL_CAMERA_ADDR 0xB6F028

#define RW_ENGINE_INSTANCE_ADDR 0xC97B24
#define RW_ENGINE_OPEN_DEVICE_OFFSET 0x10

CScene* GetGlobalScene();
CCamera* GetGlobalCamera();
RwDevice* GetOpenDevice();
bool IsGameLoaded();
void SetMatrixRotation(CMatrix* matrix, float rotX, float rotY, float rotZ);