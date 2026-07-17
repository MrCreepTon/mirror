#include "game_functions.h"

void* __cdecl RwRasterCreate(int width, int height, int depth, int flags) {
    RwRasterCreate_t func = (RwRasterCreate_t)RWRASTER_CREATE_ADDR;
    return func(width, height, depth, flags);
}

signed int __cdecl RwRasterDestroy(void* raster) {
    RwRasterDestroy_t func = (RwRasterDestroy_t)RWRASTER_DESTROY_ADDR;
    return func(raster);
}

RwCamera* __cdecl RwCameraClear(RwCamera* camera, RwRGBA* color, int clearMode) {
    RwCameraClear_t func = (RwCameraClear_t)RWCAMERA_CLEAR_ADDR;
    return func(camera, color, clearMode);
}

RwCamera* __cdecl RwCameraBeginUpdate(RwCamera* camera) {
    RwCameraBeginUpdate_t func = (RwCameraBeginUpdate_t)RWCAMERA_BEGIN_UPDATE_ADDR;
    return func(camera);
}

RwCamera* __cdecl RwCameraEndUpdate(RwCamera* camera) {
    RwCameraEndUpdate_t func = (RwCameraEndUpdate_t)RWCAMERA_END_UPDATE_ADDR;
    return func(camera);
}

void __cdecl RenderScene() {
    RenderScene_t func = (RenderScene_t)RENDER_SCENE_ADDR;
    func();
}

char __fastcall CCamera__CopyCameraMatrixToRwCam(CCamera* camera, char dontStoreOldMatrix) {
    CCamera__CopyCameraMatrixToRwCam_t func = (CCamera__CopyCameraMatrixToRwCam_t)CCAMERA_COPY_CAMERA_MATRIX_TO_RWCAM;
    return func(camera, dontStoreOldMatrix);
}

int __cdecl CSprite2D__SetVertices(CRect* rect, RwRGBA* color1, RwRGBA* color2, RwRGBA* color3, RwRGBA* color4) {
    CSprite2D__SetVertices_t func = (CSprite2D__SetVertices_t)CSPRITE2D_SET_VERTICES_ADDR;
    return func(rect, color1, color2, color3, color4);
}

void* __cdecl RwIm3DTransform(void* vertices, int numVertices, CMatrix* matrix, unsigned int flags) {
    RwIm3DTransform_t func = (RwIm3DTransform_t)RWIM3DTRANSFORM_ADDR;
    return func(vertices, numVertices, matrix, flags);
}

signed int __cdecl RwIm3DRenderIndexedPrimitive(int primitiveType, signed short* indices, signed int numIndices) {
    RwIm3DRenderIndexedPrimitive_t func = (RwIm3DRenderIndexedPrimitive_t)RWIM3DRENDERINDEXEDPRIMITIVE_ADDR;
    return func(primitiveType, indices, numIndices);
}

int __cdecl RwIm3DEnd() {
    RwIm3DEnd_t func = (RwIm3DEnd_t)RWIM3DEND_ADDR;
    return func();
}

void __cdecl CRenderer__ConstructRenderList() {
    CRenderer__ConstructRenderList_t func = (CRenderer__ConstructRenderList_t)CRENDERER_CONSTRUCT_RENDER_LIST_ADDR;
    func();
}

void __cdecl CRenderer__PreRender() {
    CRenderer__PreRender_t func = (CRenderer__PreRender_t)CRENDERER_PRERENDER_ADDR;
    func();
}

void __fastcall CCamera__CalculateDerivedValues(CCamera* camera, char a2, char a3) {
    CCamera__CalculateDerivedValues_t func = (CCamera__CalculateDerivedValues_t)CCAMERA_CALCULATE_DERIVED_VALUES;
    func(camera, a2, a3);
}

void* __cdecl RenderWeaponPedsForPC() {
    RenderWeaponPedsForPC_t func = (RenderWeaponPedsForPC_t)RENDER_WEAPON_PEDS_FOR_PC_ADDR;
    return func();
}

void __cdecl RenderEffects() {
    RenderEffects_t func = (RenderEffects_t)RENDER_EFFECTS_ADDR;
    func();
}

void __cdecl DefinedState() {
    DefinedState_t func = (DefinedState_t)DEFINED_STATE_ADDR;
    func();
}

void __cdecl RenderSkyPolys() {
    RenderSkyPolys_t func = (RenderSkyPolys_t)RENDER_SKY_POLYS_ADDR;
    func();
}