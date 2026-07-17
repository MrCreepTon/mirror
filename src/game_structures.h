#pragma once

struct RwCamera {
    char object[20];         // RwObjectHasFrame (20 байт)
    int projectionType;        // RwCameraProjection
    void* beginUpdate;         // RwCameraBeginUpdateFunc (offset)
    void* endUpdate;           // RwCameraEndUpdateFunc (offset)
    char viewMatrix[64];          // RwMatrixTag
    void* frameBuffer;         // RwRaster*
    void* zBuffer;             // RwRaster*
    float viewWindow[2];
    float recipViewWindow[2];
    float viewOffset[2];
    float nearPlane;
    float farPlane;
    float fogPlane;
    float zScale;
    float zShift;
    char frustumPlanes[120];    // RwFrustumPlane
    char frustumBoundBox[24];     // RwBBox
    char frustumCorners[96];   // RwV3d
};

struct RwRGBA {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

struct CScene {
    void* m_pRwWorld;
    RwCamera* m_pRwCamera;
};

struct CVector {
    float x;
    float y;
    float z;
};

struct CMatrix {
    CVector right;
    unsigned int flags;
    CVector up;
    unsigned int pad1;
    CVector at;
    unsigned int pad2;
    CVector pos;
    unsigned int pad3;
    void* attachMatrix;
    bool ownsAttachedMatrix;
};

struct CCamera {
    char pad[2420];
    CMatrix m_mCameraMatrix;
    char pad2[2492];
    float m_fFOV;
};

typedef void (__cdecl* RenderStateSetFunc_t)(int, unsigned int);
typedef int (__cdecl* fpIm2DRenderPrimitive_t)(int, void*, int);

struct RwDevice {
    char pad[16];
    RenderStateSetFunc_t fpRenderStateSet;
    char pod2[12];
    fpIm2DRenderPrimitive_t fpIm2DRenderPrimitive;
};

struct CRect {
    float left;
    float bottom;
    float right;
    float top;
};

struct RxObjSpace3dVertex {
    CVector pos;
    CVector objNormal;
    unsigned int color;
    float u;
    float v;
};