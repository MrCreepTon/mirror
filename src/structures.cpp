#include "structures.h"
#include "game_functions.h"
#include "rwconst.h"

Camera::Camera(int width, int height, int depth, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float fov) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->posX = posX;
    this->posY = posY;
    this->posZ = posZ;
    this->rotX = rotX;
    this->rotY = rotY;
    this->rotZ = rotZ;
    this->fov = fov;
    this->frameBuffer = RwRasterCreate(this->width, this->height, this->depth, rwRASTERTYPECAMERATEXTURE);
    this->zBuffer = RwRasterCreate(this->width, this->height, this->depth, rwRASTERTYPEZBUFFER);
}

Camera::~Camera() {
    RwRasterDestroy(this->frameBuffer);
    RwRasterDestroy(this->zBuffer);
}