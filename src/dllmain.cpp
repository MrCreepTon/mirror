#include "main.h"
#include "game_structures.h"

std::unordered_map<std::string, lua_State*> scriptStates;
std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Camera>>> scriptCameras;
std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Screen2D>>> scriptScreens2D;
std::unordered_map<std::string, std::unordered_set<std::shared_ptr<Screen3D>>> scriptScreens3D;

std::string GetCurrentScriptName(sol::this_state ts) {
    sol::state_view lua(ts);
    sol::object thisScriptObj = lua["thisScript"]();
    sol::table thisScript = thisScriptObj.as<sol::table>();
    std::string scriptName = thisScript["name"].get<std::string>();
    return scriptName;
}

void FullUnload() {
    UnloadHooks();
}

void UnloadScript(sol::this_state ts) {
    std::string scriptName = GetCurrentScriptName(ts);
    scriptCameras.erase(scriptName);
    scriptScreens2D.erase(scriptName);
    scriptScreens3D.erase(scriptName);
    scriptStates.erase(scriptName);
}

std::shared_ptr<Camera> CreateCamera(sol::this_state ts, int width, int height, int depth, sol::optional<float> posXOpt, sol::optional<float> posYOpt, sol::optional<float> posZOpt, sol::optional<float> rotXOpt, sol::optional<float> rotYOpt, sol::optional<float> rotZOpt, sol::optional<float> fovOpt) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto newCamera = std::make_shared<Camera>(
        width, height, depth,
        posXOpt.value_or(0.0f), posYOpt.value_or(0.0f), posZOpt.value_or(0.0f),
        rotXOpt.value_or(0.0f), rotYOpt.value_or(0.0f), rotZOpt.value_or(0.0f),
        fovOpt.value_or(90.0f)
    );
    scriptCameras[scriptName].insert(newCamera);
    return newCamera;
}

std::shared_ptr<Screen2D> CreateScreen2D(sol::this_state ts, float left, float top, float right, float bottom, std::optional<std::shared_ptr<Camera>> cameraOpt) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto newScreen2D = std::make_shared<Screen2D>(Screen2D{
        cameraOpt.value_or(nullptr),
        left,
        top,
        right,
        bottom
    });
    scriptScreens2D[scriptName].insert(newScreen2D);
    return newScreen2D;
}

std::shared_ptr<Screen3D> CreateScreen3D(sol::this_state ts, float leftBottomX, float leftBottomY, float leftBottomZ, float rightBottomX, float rightBottomY, float rightBottomZ, float leftTopX, float leftTopY, float leftTopZ, float rightTopX, float rightTopY, float rightTopZ, std::optional<std::shared_ptr<Camera>> cameraOpt) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto newScreen3D = std::make_shared<Screen3D>(Screen3D{
        cameraOpt.value_or(nullptr),
        CVector{leftBottomX, leftBottomY, leftBottomZ},
        CVector{rightBottomX, rightBottomY, rightBottomZ},
        CVector{leftTopX, leftTopY, leftTopZ},
        CVector{rightTopX, rightTopY, rightTopZ}
    });
    scriptScreens3D[scriptName].insert(newScreen3D);
    return newScreen3D;
}

void DeleteCamera(sol::this_state ts, std::shared_ptr<Camera> camera) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto it = scriptCameras.find(scriptName);
    if (it != scriptCameras.end()) {
        auto& camSet = it->second;
        auto camIt = camSet.find(camera);
        if (camIt != camSet.end()) {
            camSet.erase(camIt);
        }
        if (camSet.empty()) {
            scriptCameras.erase(it);
        }
    }
}

void DeleteScreen2D(sol::this_state ts, std::shared_ptr<Screen2D> screen2D) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto it = scriptScreens2D.find(scriptName);
    if (it != scriptScreens2D.end()) {
        auto& screenSet = it->second;
        auto screenIt = screenSet.find(screen2D);
        if (screenIt != screenSet.end()) {
            screenSet.erase(screenIt);
        }
        if (screenSet.empty()) {
            scriptScreens2D.erase(it);
        }
    }
}

void DeleteScreen3D(sol::this_state ts, std::shared_ptr<Screen3D> screen3D) {
    std::string scriptName = GetCurrentScriptName(ts);
    auto it = scriptScreens3D.find(scriptName);
    if (it != scriptScreens3D.end()) {
        auto& screenSet = it->second;
        auto screenIt = screenSet.find(screen3D);
        if (screenIt != screenSet.end()) {
            screenSet.erase(screenIt);
        }
        if (screenSet.empty()) {
            scriptScreens3D.erase(it);
        }
    }
}

void CallCameraHandler(const std::shared_ptr<Camera>& pCamera, const std::string& scriptName) {
    if (!pCamera) return;
    auto stateIt = scriptStates.find(scriptName);
    if (stateIt == scriptStates.end()) return;
    sol::state_view lua(stateIt->second);
    sol::protected_function handler = lua["onCameraRender"];
    if (!handler.valid()) return;
    handler(pCamera);
}

sol::table open(sol::state_view lua) {
    sol::register_main_thread(lua.lua_state());
    sol::table module = lua.create_table();

    std::string scriptName = GetCurrentScriptName(sol::this_state(lua.lua_state()));
    scriptStates[scriptName] = lua.lua_state();

    module.set("version", "1.0.2");

    module.set_function("createCamera", &CreateCamera);
    module.set_function("createScreen2D", &CreateScreen2D);
    module.set_function("createScreen3D", &CreateScreen3D);
    module.set_function("unload", &UnloadScript);

    module.new_usertype<CVector>("CVector",
        "x", &CVector::x,
        "y", &CVector::y,
        "z", &CVector::z
    );

    module.new_usertype<Camera>("Camera",
        "width", &Camera::width,
        "height", &Camera::height,
        "depth", &Camera::depth,
        "posX", &Camera::posX,
        "posY", &Camera::posY,
        "posZ", &Camera::posZ,
        "rotX", &Camera::rotX,
        "rotY", &Camera::rotY,
        "rotZ", &Camera::rotZ,
        "fov", &Camera::fov,
        "frameBuffer", &Camera::frameBuffer,
        "delete", &DeleteCamera
    );

    module.new_usertype<Screen2D>("Screen2D",
        "pCamera", &Screen2D::pCamera,
        "left", &Screen2D::left,
        "top", &Screen2D::top,
        "right", &Screen2D::right,
        "bottom", &Screen2D::bottom,
        "delete", &DeleteScreen2D
    );

    module.new_usertype<Screen3D>("Screen3D",
        "pCamera", &Screen3D::pCamera,
        "leftBottom", &Screen3D::leftBottom,
        "rightBottom", &Screen3D::rightBottom,
        "leftTop", &Screen3D::leftTop,
        "rightTop", &Screen3D::rightTop,
        "delete", &DeleteScreen3D
    );

    return module;
}

SOL_MODULE_ENTRYPOINT(open)

DWORD WINAPI OnLoad(LPVOID) {
    while (!IsGameLoaded()) {
        Sleep(1);
    }
    InitHooks();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, OnLoad, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        FullUnload();
        break;
    }
    return TRUE;
}
