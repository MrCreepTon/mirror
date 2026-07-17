#pragma once

#include "game_structures.h"

#define CMIRRORS_BEFORE_MAIN_RENDER_ADDR 0x727140
#define RENDER_2D_STUFF_ADDR 0x53E230
#define DRAW_BLUR_ADDR 0x7030A0

typedef void (__cdecl *CMirrors__BeforeMainRender_t)();
typedef void (__cdecl *Render2DStuff_t)();
typedef char (__cdecl *DrawBlur_t)(float a1);

void InitHooks();
void UnloadHooks();
void __cdecl HookedCMirrors_BeforeMainRender();
void __cdecl HookedRender2DStuff();
char __cdecl HookedDrawBlur(float a1);