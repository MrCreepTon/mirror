#include "hooks.h"
#include <MinHook.h>
#include <cmath>
#include "main.h"
#include "rwconst.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define COLOR_TOP_ADDR 0xB72CA0
#define INFARED_MODE_ADDR 0xC402B9
#define NIGHT_VISION_MODE_ADDR 0xC402B8

CMirrors__BeforeMainRender_t originalCMirrors_BeforeMainRender = nullptr;
Render2DStuff_t originalRender2DStuff = nullptr;
DrawBlur_t originalRenderBlur = nullptr;

bool cameraRender = false;

void RenderCamera(const std::shared_ptr<Camera>& pCamera, const std::string& scriptName) {
    cameraRender = true;
    CScene* scene = GetGlobalScene();
    void* oldFrameBuffer = scene->m_pRwCamera->frameBuffer;
    void* oldZBuffer = scene->m_pRwCamera->zBuffer;

    scene->m_pRwCamera->frameBuffer = pCamera->frameBuffer;
    scene->m_pRwCamera->zBuffer = pCamera->zBuffer;

    RwV2d oldViewWindow = { scene->m_pRwCamera->viewWindow[0], scene->m_pRwCamera->viewWindow[1] };

    RwRGBA color = *(RwRGBA*)COLOR_TOP_ADDR;
    int clearMode = rwCAMERACLEARZ;

    CCamera* camera = GetGlobalCamera();
    CMatrix oldMatrix = camera->m_mCameraMatrix;

    CallCameraHandler(pCamera, scriptName);

    float halfFovY = (pCamera->fov * (float)M_PI / 180.0f) * 0.5f;
    RwV2d viewWindow;
    viewWindow.y = std::tan(halfFovY);
    viewWindow.x = viewWindow.y * ((float)pCamera->width / (float)pCamera->height);
    RwCameraSetViewWindow(scene->m_pRwCamera, &viewWindow);

    SetMatrixRotation(&camera->m_mCameraMatrix, pCamera->rotX, pCamera->rotY, pCamera->rotZ);
    camera->m_mCameraMatrix.pos.x = pCamera->posX;
    camera->m_mCameraMatrix.pos.y = pCamera->posY;
    camera->m_mCameraMatrix.pos.z = pCamera->posZ;

    CCamera__CalculateDerivedValues(camera, 0, 1);
    CCamera__CopyCameraMatrixToRwCam(camera, 1);

    CRenderer__ConstructRenderList();
    CRenderer__PreRender();

    char* infaredMode = (char*)INFARED_MODE_ADDR;
    char* nightVisionMode = (char*)NIGHT_VISION_MODE_ADDR;

    char oldInfraredMode = *infaredMode;
    char oldNightVisionMode = *nightVisionMode;

    *infaredMode = 0;
    *nightVisionMode = 0;

    RwCameraClear(scene->m_pRwCamera, &color, clearMode);
    if (RwCameraBeginUpdate(scene->m_pRwCamera)) {
        DefinedState();
        RenderSkyPolys();
        RenderScene();
        RenderWeaponPedsForPC();
        RenderEffects();
        RwCameraEndUpdate(scene->m_pRwCamera);
    }

    scene->m_pRwCamera->frameBuffer = oldFrameBuffer;
    scene->m_pRwCamera->zBuffer = oldZBuffer;
    RwCameraSetViewWindow(scene->m_pRwCamera, &oldViewWindow);
    *(&camera->m_mCameraMatrix) = oldMatrix;

    *infaredMode = oldInfraredMode;
    *nightVisionMode = oldNightVisionMode;

    cameraRender = false;

    CCamera__CalculateDerivedValues(camera, 0, 1);
    CCamera__CopyCameraMatrixToRwCam(camera, 1);

    CRenderer__ConstructRenderList();
    CRenderer__PreRender();
}

void __cdecl HookedCMirrors_BeforeMainRender() {
    for (const auto& pair : scriptCameras) {
        for (const auto& cam : pair.second) {
            RenderCamera(cam, pair.first);
        }
    }
    originalCMirrors_BeforeMainRender();
}

void RenderScreen3D(Screen3D* pScreen3D) {
    if (!pScreen3D->pCamera) {
        return;
    }

    RwDevice* device = GetOpenDevice();

    RxObjSpace3dVertex vertices[4];

    vertices[0].pos = pScreen3D->leftTop;
    vertices[0].objNormal = { 0.0f, 0.0f, 0.0f };
    vertices[0].color = 0xFFFFFFFF;
    vertices[0].u = 0.0f; vertices[0].v = 0.0f;

    vertices[1].pos = pScreen3D->rightTop;
    vertices[1].objNormal = { 0.0f, 0.0f, 0.0f };
    vertices[1].color = 0xFFFFFFFF;
    vertices[1].u = 1.0f; vertices[1].v = 0.0f;

    vertices[2].pos = pScreen3D->leftBottom;
    vertices[2].objNormal = { 0.0f, 0.0f, 0.0f };
    vertices[2].color = 0xFFFFFFFF;
    vertices[2].u = 0.0f; vertices[2].v = 1.0f;

    vertices[3].pos = pScreen3D->rightBottom;
    vertices[3].objNormal = { 0.0f, 0.0f, 0.0f };
    vertices[3].color = 0xFFFFFFFF;
    vertices[3].u = 1.0f; vertices[3].v = 1.0f;

    INT16 indices[6] = { 0, 2, 1, 1, 2, 3 };

    device->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    device->fpRenderStateSet(rwRENDERSTATESRCBLEND, 5);
    device->fpRenderStateSet(rwRENDERSTATEDESTBLEND, 6);
    device->fpRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
    device->fpRenderStateSet(rwRENDERSTATECULLMODE, 1);
    if (pScreen3D->pCamera) {
        device->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (unsigned int)pScreen3D->pCamera->frameBuffer);
    }

    if (RwIm3DTransform(vertices, 4, 0, 1u)) {
        RwIm3DRenderIndexedPrimitive(3, indices, 6);
        RwIm3DEnd();
    }

    device->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
}

void RenderScreen2D(Screen2D* pScreen2D) {
    RwDevice* device = GetOpenDevice();
    CRect rect = { pScreen2D->left, pScreen2D->bottom, pScreen2D->right, pScreen2D->top };
    RwRGBA color = { 255, 255, 255, 255 };
    CSprite2D__SetVertices(&rect, &color, &color, &color, &color);
    device->fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 1);
    device->fpRenderStateSet(rwRENDERSTATESRCBLEND, 5);
    device->fpRenderStateSet(rwRENDERSTATEDESTBLEND, 6);
    device->fpRenderStateSet(rwRENDERSTATEFOGENABLE, 0);
    device->fpRenderStateSet(rwRENDERSTATECULLMODE, 1);
    if (pScreen2D->pCamera) {
        device->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, (unsigned int)pScreen2D->pCamera->frameBuffer);
    }
    else {
        device->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
    }
    device->fpIm2DRenderPrimitive(5, (void*)(CSPRITE_2D_MA_VERTICES_ADDR), 4);
    device->fpRenderStateSet(rwRENDERSTATETEXTURERASTER, 0);
}

void __cdecl HookedRender2DStuff() {
    for (const auto& pair : scriptScreens3D) {
        for (const auto& screen3D : pair.second) {
            RenderScreen3D(screen3D.get());
        }
    }
    originalRender2DStuff();
    for (const auto& pair : scriptScreens2D) {
        for (const auto& screen2D : pair.second) {
            RenderScreen2D(screen2D.get());
        }
    }
}

char __cdecl HookedDrawBlur(float a1) {
    if (cameraRender) {
        return 0;
    }
    return originalRenderBlur(a1);
}

void InitHooks() {
    MH_Initialize();
    MH_CreateHook((void*)CMIRRORS_BEFORE_MAIN_RENDER_ADDR, &HookedCMirrors_BeforeMainRender, reinterpret_cast<LPVOID*>(&originalCMirrors_BeforeMainRender));
    MH_EnableHook((void*)CMIRRORS_BEFORE_MAIN_RENDER_ADDR);

    MH_CreateHook((void*)RENDER_2D_STUFF_ADDR, &HookedRender2DStuff, reinterpret_cast<LPVOID*>(&originalRender2DStuff));
    MH_EnableHook((void*)RENDER_2D_STUFF_ADDR);

    MH_CreateHook((void*)DRAW_BLUR_ADDR, &HookedDrawBlur, reinterpret_cast<LPVOID*>(&originalRenderBlur));
    MH_EnableHook((void*)DRAW_BLUR_ADDR);
}

void UnloadHooks() {
    MH_DisableHook((void*)CMIRRORS_BEFORE_MAIN_RENDER_ADDR);
    MH_DisableHook((void*)RENDER_2D_STUFF_ADDR);
    MH_DisableHook((void*)DRAW_BLUR_ADDR);
    MH_Uninitialize();
}