#pragma once

#include "game_structures.h"

#define RWRASTER_CREATE_ADDR 0x7FB230
#define RWRASTER_DESTROY_ADDR 0x7FB020
#define RWCAMERA_CLEAR_ADDR 0x7EE340
#define RWCAMERA_BEGIN_UPDATE_ADDR 0x619450
#define RWCAMERA_END_UPDATE_ADDR 0x7EE180
#define RENDER_SCENE_ADDR 0x53DF40
#define CCAMERA_COPY_CAMERA_MATRIX_TO_RWCAM 0x50AFA0
#define CSPRITE2D_SET_VERTICES_ADDR 0x727420
#define RWIM3DTRANSFORM_ADDR 0x7EF450
#define RWIM3DRENDERINDEXEDPRIMITIVE_ADDR 0x7EF550
#define RWIM3DEND_ADDR 0x7EF520
#define CRENDERER_CONSTRUCT_RENDER_LIST_ADDR 0x5556E0
#define CRENDERER_PRERENDER_ADDR 0x553910
#define CCAMERA_CALCULATE_DERIVED_VALUES 0x5150E0
#define RENDER_WEAPON_PEDS_FOR_PC_ADDR 0x732F30
#define RENDER_EFFECTS_ADDR 0x53E170
#define DEFINED_STATE_ADDR 0x734650
#define RENDER_SKY_POLYS_ADDR 0x714650

typedef void* (__cdecl *RwRasterCreate_t)(int width, int height, int depth, int flags);
typedef signed int (__cdecl *RwRasterDestroy_t)(void* raster);
typedef RwCamera* (__cdecl *RwCameraClear_t)(RwCamera* camera, RwRGBA* color, int clearMode);
typedef RwCamera* (__cdecl *RwCameraBeginUpdate_t)(RwCamera* camera);
typedef RwCamera* (__cdecl *RwCameraEndUpdate_t)(RwCamera* camera);
typedef void (__cdecl *RenderScene_t)();
typedef char (__thiscall *CCamera__CopyCameraMatrixToRwCam_t)(CCamera* camera, char dontStoreOldMatrix);
typedef int (__cdecl *CSprite2D__SetVertices_t)(CRect* rect, RwRGBA* color1, RwRGBA* color2, RwRGBA* color3, RwRGBA* color4);
typedef void* (__cdecl* RwIm3DTransform_t)(void* vertices, int numVertices, CMatrix* matrix, unsigned int flags);
typedef signed int (__cdecl* RwIm3DRenderIndexedPrimitive_t)(int primitiveType, signed short* indices, signed int numIndices);
typedef int (__cdecl* RwIm3DEnd_t)();
typedef void (__cdecl* CRenderer__ConstructRenderList_t)();
typedef void (__cdecl* CRenderer__PreRender_t)();
typedef void (__thiscall* CCamera__CalculateDerivedValues_t)(CCamera* camera, char a2, char a3);
typedef void* (__cdecl* RenderWeaponPedsForPC_t)();
typedef void (__cdecl* RenderEffects_t)();
typedef void (__cdecl* DefinedState_t)();
typedef void (__cdecl* RenderSkyPolys_t)();

void* __cdecl RwRasterCreate(int width, int height, int depth, int flags);
signed int __cdecl RwRasterDestroy(void* raster);
RwCamera* __cdecl RwCameraClear(RwCamera* camera, RwRGBA* color, int clearMode);
RwCamera* __cdecl RwCameraBeginUpdate(RwCamera* camera);
RwCamera* __cdecl RwCameraEndUpdate(RwCamera* camera);
void __cdecl RenderScene();
char __fastcall CCamera__CopyCameraMatrixToRwCam(CCamera* camera, char dontStoreOldMatrix);
int __cdecl CSprite2D__SetVertices(CRect* rect, RwRGBA* color1, RwRGBA* color2, RwRGBA* color3, RwRGBA* color4);
void* __cdecl RwIm3DTransform(void* vertices, int numVertices, CMatrix* matrix, unsigned int flags);
signed int __cdecl RwIm3DRenderIndexedPrimitive(int primitiveType, signed short* indices, signed int numIndices);
int __cdecl RwIm3DEnd();
void __cdecl CRenderer__ConstructRenderList();
void __cdecl CRenderer__PreRender();
void __fastcall CCamera__CalculateDerivedValues(CCamera* camera, char a2, char a3);
void* __cdecl RenderWeaponPedsForPC();
void __cdecl RenderEffects();
void __cdecl DefinedState();
void __cdecl RenderSkyPolys();