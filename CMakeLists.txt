cmake_minimum_required(VERSION 3.21)

project(mirror_core VERSION 0.1.0 LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "mirror_core targets 32-bit Windows and must be built with MSVC.")
endif()
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "mirror_core must be built as 32-bit. Configure with: cmake --preset win32")
endif()

option(MIRROR_WITH_STREAMING "Run CStreaming::Update during off-screen camera rendering" OFF)
set(MIRROR_DEPLOY_DIR "" CACHE PATH "Optional: copy the built DLL here after each build (e.g. <game>/moonloader/lib)")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

include(FetchContent)
FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG        v1.3.4
)
FetchContent_MakeAvailable(minhook)

add_library(mirror_core SHARED
    src/dllmain.cpp
    src/game_functions.cpp
    src/hooks.cpp
    src/structures.cpp
    src/util.cpp
)

target_compile_features(mirror_core PRIVATE cxx_std_17)

target_compile_definitions(mirror_core PRIVATE
    MODULE_NAME=mirror_core
    SOL_ALL_SAFETIES_ON=1
    SOL_LUAJIT=1
    _CRT_SECURE_NO_WARNINGS
    $<$<BOOL:${MIRROR_WITH_STREAMING}>:MIRROR_WITH_STREAMING>
)

target_include_directories(mirror_core PRIVATE
    ${CMAKE_SOURCE_DIR}/include/lua
    ${CMAKE_SOURCE_DIR}/include/sol2
)

target_link_libraries(mirror_core PRIVATE
    minhook
    ${CMAKE_SOURCE_DIR}/lib/lua51.lib
)

set_target_properties(mirror_core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/bin>"
)

if(MIRROR_DEPLOY_DIR)
    add_custom_command(TARGET mirror_core POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${MIRROR_DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:mirror_core>" "${MIRROR_DEPLOY_DIR}/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/lua/mirror.lua" "${MIRROR_DEPLOY_DIR}/"
        COMMENT "Deploying $<TARGET_FILE_NAME:mirror_core> and mirror.lua to ${MIRROR_DEPLOY_DIR}"
    )
endif()
