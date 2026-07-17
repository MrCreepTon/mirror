#pragma once

#include <windows.h>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol.hpp>
#include "hooks.h"
#include "game_functions.h"
#include "util.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "structures.h"

#define LUAOPEN_MODULE_EXPAND(name) luaopen_##name
#define LUAOPEN_MODULE(name) LUAOPEN_MODULE_EXPAND(name)
#define SOL_MODULE_ENTRYPOINT(func) extern "C" __declspec(dllexport) int LUAOPEN_MODULE(MODULE_NAME)(lua_State* L) { \
    sol::state_view lua(L); \
    return sol::stack::push(L, func(lua)); \
}

#define CSPRITE_2D_MA_VERTICES_ADDR 0xC80468

extern std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Camera>>> scriptCameras;
extern std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Screen2D>>> scriptScreens2D;
extern std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Screen3D>>> scriptScreens3D;