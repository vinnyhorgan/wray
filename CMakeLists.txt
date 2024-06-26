cmake_minimum_required(VERSION 3.14)
project(wray C)
set(CMAKE_C_STANDARD 99)

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

if(MSVC)
    add_compile_options(/wd4244 /wd4267 /wd4996)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(SUPPORT_MODULE_RMODELS OFF CACHE BOOL "" FORCE)
set(SUPPORT_CAMERA_SYSTEM OFF CACHE BOOL "" FORCE)
set(SUPPORT_GESTURES_SYSTEM OFF CACHE BOOL "" FORCE)
set(SUPPORT_RPRAND_GENERATOR OFF CACHE BOOL "" FORCE)
set(SUPPORT_MOUSE_GESTURES OFF CACHE BOOL "" FORCE)
set(SUPPORT_SCREEN_CAPTURE OFF CACHE BOOL "" FORCE)
set(SUPPORT_GIF_RECORDING OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_DDS OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_HDR OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_BMP ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_JPG ON CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_GIF OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_QOI OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_XM OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_MOD OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_QOA OFF CACHE BOOL "" FORCE)
set(SUPPORT_FILEFORMAT_FLAC ON CACHE BOOL "" FORCE)

FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

FetchContent_Declare(enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG v1.3.18
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(enet)
include_directories(${enet_SOURCE_DIR}/include)

set(SOURCES
    src/api.c
    src/net.c
    src/util.c
    src/wray.c
    src/lib/argparse/argparse.c
    src/lib/map/map.c
    src/lib/microui/microui.c
    src/lib/naett/naett.c
    src/lib/wren/wren.c
    src/lib/zip/zip.c
)

if(WIN32)
    list(APPEND SOURCES assets/wray.rc)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} raylib enet)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} winhttp ws2_32)
endif()

if(UNIX)
    target_link_libraries(${PROJECT_NAME} curl)
endif()
