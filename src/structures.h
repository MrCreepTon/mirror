#pragma once

#include <memory>

#include "game_structures.h"

class Camera {
public:
    Camera(int width, int height, int depth, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov);
    ~Camera();
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    int width;
    int height;
    int depth;
    float posX;
    float posY;
    float posZ;
    float rotX;
    float rotY;
    float rotZ;
    void* frameBuffer;
    void* zBuffer;
    float fov;
};

struct Screen2D {
    std::shared_ptr<Camera> pCamera;
    float left;
    float top;
    float right;
    float bottom;
};

struct Screen3D {
    std::shared_ptr<Camera> pCamera;
    CVector leftBottom;
    CVector rightBottom;
    CVector leftTop;
    CVector rightTop;
};