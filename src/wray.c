#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "lib/argparse/argparse.h"
#include "lib/wren/wren.h"
#include "lib/zip/zip.h"

#include "api.h"
#include "api.wren.h"
#include "util.h"

#ifdef _WIN32
#include <direct.h>
#define mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define mkdir(path) mkdir(path, 0777)
#endif

static char selfPath[256];
static struct zip_t* egg = NULL;

static unsigned char* zipLoadFileData(const char* path, int* size)
{
    if (zip_entry_open(egg, path) < 0)
        return NULL;

    *size = (int)zip_entry_size(egg);

    unsigned char* buffer = (unsigned char*)malloc(*size);
    if (buffer == NULL) {
        zip_entry_close(egg);
        return NULL;
    }

    zip_entry_noallocread(egg, buffer, *size);

    zip_entry_close(egg);

    return buffer;
}

static char* zipLoadFileText(const char* path)
{
    if (zip_entry_open(egg, path) < 0)
        return NULL;

    size_t size = zip_entry_size(egg);

    char* buffer = (char*)malloc(size + 1);
    if (buffer == NULL) {
        zip_entry_close(egg);
        return NULL;
    }

    zip_entry_noallocread(egg, buffer, size);

    buffer[size] = '\0';

    zip_entry_close(egg);

    return buffer;
}

static void onComplete(WrenVM* vm, const char* name, WrenLoadModuleResult result)
{
    if (result.source)
        free((void*)result.source);
}

static WrenLoadModuleResult wrenLoadModule(WrenVM* vm, const char* name)
{
    WrenLoadModuleResult result = { 0 };

    if (TextIsEqual(name, "meta") || TextIsEqual(name, "random"))
        return result;

    if (TextIsEqual(name, "wray")) {
        result.source = apiModuleSource;
        return result;
    }

    result.source = LoadFileText(TextFormat("%s.wren", name));
    result.onComplete = onComplete;

    return result;
}

#define BIND_METHOD(s, m)          \
    if (TextIsEqual(signature, s)) \
        return m;

static WrenForeignMethodFn wrenBindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature)
{
    if (TextIsEqual(className, "Audio")) {
        BIND_METHOD("init()", audioInit);
        BIND_METHOD("volume", audioGetVolume);
        BIND_METHOD("volume=(_)", audioSetVolume);
    } else if (TextIsEqual(className, "Sound")) {
        BIND_METHOD("init new(_)", soundNew);
        BIND_METHOD("play()", soundPlay);
        BIND_METHOD("stop()", soundStop);
        BIND_METHOD("pause()", soundPause);
        BIND_METHOD("resume()", soundResume);
        BIND_METHOD("playing", soundGetPlaying);
        BIND_METHOD("volume=(_)", soundSetVolume);
        BIND_METHOD("pitch=(_)", soundSetPitch);
        BIND_METHOD("pan=(_)", soundSetPan);
    } else if (TextIsEqual(className, "SoundAlias")) {
        BIND_METHOD("init new(_)", soundAliasNew);
        BIND_METHOD("play()", soundPlay);
        BIND_METHOD("stop()", soundStop);
        BIND_METHOD("pause()", soundPause);
        BIND_METHOD("resume()", soundResume);
        BIND_METHOD("playing", soundGetPlaying);
        BIND_METHOD("volume=(_)", soundSetVolume);
        BIND_METHOD("pitch=(_)", soundSetPitch);
        BIND_METHOD("pan=(_)", soundSetPan);
    } else if (TextIsEqual(className, "Graphics")) {
        BIND_METHOD("begin()", graphicsBegin);
        BIND_METHOD("end()", graphicsEnd);
        BIND_METHOD("beginBlend(_)", graphicsBeginBlend);
        BIND_METHOD("endBlend()", graphicsEndBlend);
        BIND_METHOD("beginScissor(_,_,_,_)", graphicsBeginScissor);
        BIND_METHOD("endScissor()", graphicsEndScissor);
        BIND_METHOD("screenshot(_)", graphicsScreenshot);
        BIND_METHOD("measure(_,_)", graphicsMeasure);
        BIND_METHOD("noise(_,_,_,_)", graphicsNoise);
        BIND_METHOD("clear(_)", graphicsClear);
        BIND_METHOD("print(_,_,_,_,_)", graphicsPrint);
        BIND_METHOD("pixel(_,_,_)", graphicsPixel);
        BIND_METHOD("line(_,_,_,_,_,_)", graphicsLine);
        BIND_METHOD("lineBezier(_,_,_,_,_,_)", graphicsLineBezier);
        BIND_METHOD("circle(_,_,_,_)", graphicsCircle);
        BIND_METHOD("circleLines(_,_,_,_)", graphicsCircleLines);
        BIND_METHOD("ellipse(_,_,_,_,_)", graphicsEllipse);
        BIND_METHOD("ellipseLines(_,_,_,_,_)", graphicsEllipseLines);
        BIND_METHOD("rectangle(_,_,_,_,_,_,_,_)", graphicsRectangle);
        BIND_METHOD("rectangleLines(_,_,_,_,_,_)", graphicsRectangleLines);
        BIND_METHOD("triangle(_,_,_,_,_,_,_)", graphicsTriangle);
        BIND_METHOD("triangleLines(_,_,_,_,_,_,_)", graphicsTriangleLines);
        BIND_METHOD("polygon(_,_,_,_,_,_)", graphicsPolygon);
        BIND_METHOD("polygonLines(_,_,_,_,_,_,_)", graphicsPolygonLines);
        BIND_METHOD("noiseSeed=(_)", graphicsSetNoiseSeed);
        BIND_METHOD("lineSpacing=(_)", graphicsSetLineSpacing);
    } else if (TextIsEqual(className, "UI")) {
        BIND_METHOD("update()", uiUpdate);
        BIND_METHOD("draw()", uiDraw);
        BIND_METHOD("begin()", uiBegin);
        BIND_METHOD("end()", uiEnd);
        BIND_METHOD("beginWindow(_,_,_,_,_)", uiBeginWindow);
        BIND_METHOD("endWindow()", uiEndWindow);
        BIND_METHOD("label(_)", uiLabel);
    } else if (TextIsEqual(className, "Color")) {
        BIND_METHOD("init new(_,_,_,_)", colorNew);
        BIND_METHOD("init new(_,_,_)", colorNew2);
        BIND_METHOD("[_]", colorGetIndex);
        BIND_METHOD("[_]=(_)", colorSetIndex);
    } else if (TextIsEqual(className, "Image")) {
        BIND_METHOD("init new(_)", imageNew);
        BIND_METHOD("init new(_,_,_)", imageNew2);
        BIND_METHOD("init fromScreen()", imageNew3);
        BIND_METHOD("init fromImage(_,_,_,_,_)", imageNew4);
        BIND_METHOD("init gradientLinear(_,_,_,_,_)", imageNew5);
        BIND_METHOD("init gradientRadial(_,_,_,_,_)", imageNew6);
        BIND_METHOD("init gradientSquare(_,_,_,_,_)", imageNew7);
        BIND_METHOD("export(_)", imageExport);
        BIND_METHOD("crop(_,_,_,_)", imageCrop);
        BIND_METHOD("resize(_,_)", imageResize);
        BIND_METHOD("flipVertical()", imageFlipVertical);
        BIND_METHOD("flipHorizontal()", imageFlipHorizontal);
        BIND_METHOD("rotate(_)", imageRotate);
        BIND_METHOD("width", imageGetWidth);
        BIND_METHOD("height", imageGetHeight);
        BIND_METHOD("format", imageGetFormat);
    } else if (TextIsEqual(className, "Texture")) {
        BIND_METHOD("init new(_)", textureNew);
        BIND_METHOD("draw(_,_,_,_,_,_,_,_)", textureDraw);
        BIND_METHOD("drawRec(_,_,_,_,_,_,_,_,_,_,_,_)", textureDrawRec);
        BIND_METHOD("width", textureGetWidth);
        BIND_METHOD("height", textureGetHeight);
        BIND_METHOD("filter=(_)", textureSetFilter);
        BIND_METHOD("wrap=(_)", textureSetWrap);
    } else if (TextIsEqual(className, "RenderTexture")) {
        BIND_METHOD("init new(_,_)", renderTextureNew);
        BIND_METHOD("begin()", renderTextureBegin);
        BIND_METHOD("end()", renderTextureEnd);
        BIND_METHOD("texture", renderTextureGetTexture);
    } else if (TextIsEqual(className, "Font")) {
        BIND_METHOD("init new(_,_)", fontNew);
        BIND_METHOD("print(_,_,_,_,_,_,_,_,_)", fontPrint);
        BIND_METHOD("measure(_)", fontMeasure);
        BIND_METHOD("size", fontGetSize);
    } else if (TextIsEqual(className, "Camera")) {
        BIND_METHOD("init new(_,_)", cameraNew);
        BIND_METHOD("begin()", cameraBegin);
        BIND_METHOD("end()", cameraEnd);
        BIND_METHOD("screenToWorld(_,_)", cameraScreenToWorld);
        BIND_METHOD("worldToScreen(_,_)", cameraWorldToScreen);
        BIND_METHOD("x", cameraGetX);
        BIND_METHOD("y", cameraGetY);
        BIND_METHOD("ox", cameraGetOffsetX);
        BIND_METHOD("oy", cameraGetOffsetY);
        BIND_METHOD("r", cameraGetRotation);
        BIND_METHOD("zoom", cameraGetZoom);
        BIND_METHOD("x=(_)", cameraSetX);
        BIND_METHOD("y=(_)", cameraSetY);
        BIND_METHOD("ox=(_)", cameraSetOffsetX);
        BIND_METHOD("oy=(_)", cameraSetOffsetY);
        BIND_METHOD("r=(_)", cameraSetRotation);
        BIND_METHOD("zoom=(_)", cameraSetZoom);
    } else if (TextIsEqual(className, "Shader")) {
        BIND_METHOD("init new(_,_)", shaderNew);
        BIND_METHOD("init new(_)", shaderNew2);
        BIND_METHOD("begin()", shaderBegin);
        BIND_METHOD("end()", shaderEnd);
        BIND_METHOD("set(_,_)", shaderSet);
    } else if (TextIsEqual(className, "Keyboard")) {
        BIND_METHOD("pressed(_)", keyboardPressed);
        BIND_METHOD("pressedRepeat(_)", keyboardPressedRepeat);
        BIND_METHOD("down(_)", keyboardDown);
        BIND_METHOD("released(_)", keyboardReleased);
        BIND_METHOD("keyPressed", keyboardGetKeyPressed);
    } else if (TextIsEqual(className, "Mouse")) {
        BIND_METHOD("pressed(_)", mousePressed);
        BIND_METHOD("down(_)", mouseDown);
        BIND_METHOD("released(_)", mouseReleased);
        BIND_METHOD("setPosition(_,_)", mouseSetPosition);
        BIND_METHOD("setOffset(_,_)", mouseSetOffset);
        BIND_METHOD("setScale(_,_)", mouseSetScale);
        BIND_METHOD("x", mouseGetX);
        BIND_METHOD("y", mouseGetY);
        BIND_METHOD("dx", mouseGetDeltaX);
        BIND_METHOD("dy", mouseGetDeltaY);
        BIND_METHOD("wheel", mouseGetWheel);
        BIND_METHOD("cursor=(_)", mouseSetCursor);
        BIND_METHOD("hidden", mouseGetHidden);
        BIND_METHOD("hidden=(_)", mouseSetHidden);
        BIND_METHOD("enabled=(_)", mouseSetEnabled);
        BIND_METHOD("onScreen", mouseGetOnScreen);
    } else if (TextIsEqual(className, "Gamepad")) {
        BIND_METHOD("available(_)", gamepadAvailable);
        BIND_METHOD("init new(_)", gamepadNew);
        BIND_METHOD("pressed(_)", gamepadPressed);
        BIND_METHOD("down(_)", gamepadDown);
        BIND_METHOD("released(_)", gamepadReleased);
        BIND_METHOD("axis(_)", gamepadAxis);
        BIND_METHOD("id", gamepadGetId);
        BIND_METHOD("name", gamepadGetName);
        BIND_METHOD("axisCount", gamepadGetAxisCount);
    } else if (TextIsEqual(className, "Window")) {
        BIND_METHOD("init(_,_,_)", windowInit);
        BIND_METHOD("toggleFullscreen()", windowToggleFullscreen);
        BIND_METHOD("toggleBorderless()", windowToggleBorderless);
        BIND_METHOD("maximize()", windowMaximize);
        BIND_METHOD("minimize()", windowMinimize);
        BIND_METHOD("restore()", windowRestore);
        BIND_METHOD("setPosition(_,_)", windowSetPosition);
        BIND_METHOD("setMinSize(_,_)", windowSetMinSize);
        BIND_METHOD("setMaxSize(_,_)", windowSetMaxSize);
        BIND_METHOD("setSize(_,_)", windowSetSize);
        BIND_METHOD("focus()", windowFocus);
        BIND_METHOD("getMonitorInfo(_)", windowGetMonitorInfo);
        BIND_METHOD("listDropped()", windowListDropped);
        BIND_METHOD("closed", windowGetClosed);
        BIND_METHOD("fullscreen", windowGetFullscreen);
        BIND_METHOD("hidden", windowGetHidden);
        BIND_METHOD("minimized", windowGetMinimized);
        BIND_METHOD("maximized", windowGetMaximized);
        BIND_METHOD("focused", windowGetFocused);
        BIND_METHOD("resized", windowGetResized);
        BIND_METHOD("width", windowGetWidth);
        BIND_METHOD("height", windowGetHeight);
        BIND_METHOD("monitorCount", windowGetMonitorCount);
        BIND_METHOD("monitor", windowGetMonitor);
        BIND_METHOD("x", windowGetX);
        BIND_METHOD("y", windowGetY);
        BIND_METHOD("dpi", windowGetDpi);
        BIND_METHOD("fileDropped", windowGetFileDropped);
        BIND_METHOD("icon=(_)", windowSetIcon);
        BIND_METHOD("title=(_)", windowSetTitle);
        BIND_METHOD("monitor=(_)", windowSetMonitor);
        BIND_METHOD("opacity=(_)", windowSetOpacity);
        BIND_METHOD("targetFps=(_)", windowSetTargetFps);
        BIND_METHOD("resizable", windowGetResizable);
        BIND_METHOD("resizable=(_)", windowSetResizable);
        BIND_METHOD("vsync", windowGetVSync);
        BIND_METHOD("vsync=(_)", windowSetVSync);
        BIND_METHOD("dt", windowGetDt);
        BIND_METHOD("time", windowGetTime);
        BIND_METHOD("fps", windowGetFps);
    } else if (TextIsEqual(className, "OS")) {
        BIND_METHOD("readLine()", osReadLine);
        BIND_METHOD("wait(_)", osWait);
        BIND_METHOD("openUrl(_)", osOpenUrl);
        BIND_METHOD("args", osGetArgs);
        BIND_METHOD("name", osGetName);
        BIND_METHOD("wrayVersion", osGetWrayVersion);
        BIND_METHOD("clipboard", osGetClipboard);
        BIND_METHOD("clipboard=(_)", osSetClipboard);
    } else if (TextIsEqual(className, "Data")) {
        BIND_METHOD("compress(_)", dataCompress);
        BIND_METHOD("decompress(_)", dataDecompress);
        BIND_METHOD("encodeBase64(_)", dataEncodeBase64);
        BIND_METHOD("decodeBase64(_)", dataDecodeBase64);
        BIND_METHOD("encodeHex(_)", dataEncodeHex);
        BIND_METHOD("decodeHex(_)", dataDecodeHex);
        BIND_METHOD("hash(_)", dataHash);
    } else if (TextIsEqual(className, "Directory")) {
        BIND_METHOD("exists(_)", directoryExists);
        BIND_METHOD("list(_)", directoryList);
    } else if (TextIsEqual(className, "File")) {
        BIND_METHOD("exists(_)", fileExists);
        BIND_METHOD("size(_)", fileSize);
        BIND_METHOD("read(_)", fileRead);
        BIND_METHOD("write(_,_)", fileWrite);
    } else if (TextIsEqual(className, "Request")) {
        BIND_METHOD("init new(_)", requestNew);
        BIND_METHOD("make()", requestMake);
        BIND_METHOD("complete", requestGetComplete);
        BIND_METHOD("status", requestGetStatus);
        BIND_METHOD("body", requestGetBody);
    } else if (TextIsEqual(className, "ENet")) {
        BIND_METHOD("init()", enetInit);
        BIND_METHOD("version", enetGetVersion);
    } else if (TextIsEqual(className, "Host")) {
        BIND_METHOD("init new(_,_,_,_,_)", hostNew);
        BIND_METHOD("connect(_,_,_)", hostConnect);
        BIND_METHOD("service(_)", hostService);
        BIND_METHOD("checkEvents()", hostCheckEvents);
        BIND_METHOD("compressWithRangeCoder()", hostCompressWithRangeCoder);
        BIND_METHOD("flush()", hostFlush);
        BIND_METHOD("broadcast(_,_,_)", hostBroadcast);
        BIND_METHOD("setBandwidthLimit(_,_)", hostSetBandwidthLimit);
        BIND_METHOD("getPeer(_)", hostGetPeer);
        BIND_METHOD("totalSent", hostGetTotalSent);
        BIND_METHOD("totalReceived", hostGetTotalReceived);
        BIND_METHOD("serviceTime", hostGetServiceTime);
        BIND_METHOD("peerCount", hostGetPeerCount);
        BIND_METHOD("address", hostGetAddress);
        BIND_METHOD("channelLimit=(_)", hostSetChannelLimit);
    } else if (TextIsEqual(className, "Peer")) {
        BIND_METHOD("disconnect(_)", peerDisconnect);
        BIND_METHOD("disconnectNow(_)", peerDisconnectNow);
        BIND_METHOD("disconnectLater(_)", peerDisconnectLater);
        BIND_METHOD("ping()", peerPing);
        BIND_METHOD("reset()", peerReset);
        BIND_METHOD("send(_,_,_)", peerSend);
        BIND_METHOD("receive()", peerReceive);
        BIND_METHOD("configThrottle(_,_,_)", peerConfigThrottle);
        BIND_METHOD("setTimeout(_,_,_)", peerSetTimeout);
        BIND_METHOD("connectId", peerGetConnectId);
        BIND_METHOD("index", peerGetIndex);
        BIND_METHOD("state", peerGetState);
        BIND_METHOD("rtt", peerGetRtt);
        BIND_METHOD("lastRtt", peerGetLastRtt);
        BIND_METHOD("timeout", peerGetTimeout);
        BIND_METHOD("toString", peerGetToString);
        BIND_METHOD("pingInterval=(_)", peerSetPingInterval);
        BIND_METHOD("rtt=(_)", peerSetRtt);
        BIND_METHOD("lastRtt=(_)", peerSetLastRtt);
    }

    return NULL;
}

static WrenForeignClassMethods wrenBindForeignClass(WrenVM* vm, const char* module, const char* className)
{
    WrenForeignClassMethods methods = { 0 };

    if (TextIsEqual(className, "Sound")) {
        methods.allocate = soundAllocate;
        methods.finalize = soundFinalize;
    } else if (TextIsEqual(className, "SoundAlias")) {
        methods.allocate = soundAllocate;
        methods.finalize = soundAliasFinalize;
    } else if (TextIsEqual(className, "Color")) {
        methods.allocate = colorAllocate;
    } else if (TextIsEqual(className, "Image")) {
        methods.allocate = imageAllocate;
        methods.finalize = imageFinalize;
    } else if (TextIsEqual(className, "Texture")) {
        methods.allocate = textureAllocate;
        methods.finalize = textureFinalize;
    } else if (TextIsEqual(className, "RenderTexture")) {
        methods.allocate = renderTextureAllocate;
        methods.finalize = renderTextureFinalize;
    } else if (TextIsEqual(className, "Font")) {
        methods.allocate = fontAllocate;
        methods.finalize = fontFinalize;
    } else if (TextIsEqual(className, "Camera")) {
        methods.allocate = cameraAllocate;
    } else if (TextIsEqual(className, "Shader")) {
        methods.allocate = shaderAllocate;
        methods.finalize = shaderFinalize;
    } else if (TextIsEqual(className, "Gamepad")) {
        methods.allocate = gamepadAllocate;
    } else if (TextIsEqual(className, "Request")) {
        methods.allocate = requestAllocate;
        methods.finalize = requestFinalize;
    } else if (TextIsEqual(className, "Host")) {
        methods.allocate = hostAllocate;
        methods.finalize = hostFinalize;
    }

    return methods;
}

static void wrenWrite(WrenVM* vm, const char* text)
{
    printf("%s", text);
}

static void wrenError(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
    switch (type) {
    case WREN_ERROR_COMPILE:
        printf("[%s line %d] %s\n", module, line, message);
        break;
    case WREN_ERROR_RUNTIME:
        printf("%s\n", message);
        break;
    case WREN_ERROR_STACK_TRACE:
        printf("[%s line %d] in %s\n", module, line, message);
        break;
    }
}

static void runWren(const char* script, const char* module)
{
    char* source = LoadFileText(script);

    WrenConfiguration config;
    wrenInitConfiguration(&config);

    config.loadModuleFn = wrenLoadModule;
    config.bindForeignMethodFn = wrenBindForeignMethod;
    config.bindForeignClassFn = wrenBindForeignClass;
    config.writeFn = wrenWrite;
    config.errorFn = wrenError;

    WrenVM* vm = wrenNewVM(&config);

    if (wrenInterpret(vm, "wray", apiModuleSource) != WREN_RESULT_SUCCESS) {
        wrenFreeVM(vm);
        UnloadFileText(source);
        return;
    }

    vmData* data = malloc(sizeof(vmData));
    data->audioInit = false;
    data->windowInit = false;
    data->enetInit = false;

    data->uiCtx = malloc(sizeof(mu_Context));
    mu_init(data->uiCtx);
    data->uiCtx->text_width = uiTextWidth;
    data->uiCtx->text_height = uiTextHeight;

    loadKeys(&data->keys);

    wrenEnsureSlots(vm, 1);
    wrenGetVariable(vm, "wray", "Texture", 0);
    data->textureClass = wrenGetSlotHandle(vm, 0);
    wrenGetVariable(vm, "wray", "Peer", 0);
    data->peerClass = wrenGetSlotHandle(vm, 0);

    wrenSetUserData(vm, data);

    wrenInterpret(vm, module, source);

    wrenReleaseHandle(vm, data->textureClass);
    wrenReleaseHandle(vm, data->peerClass);

    map_deinit(&data->keys);

    free(data->uiCtx);

    UnloadFileText(source);
    wrenFreeVM(vm);

    if (data->audioInit)
        CloseAudioDevice();
    if (data->windowInit)
        CloseWindow();
    if (data->enetInit)
        enetClose();

    free(data);
}

typedef struct {
    const char* name;
    int (*fn)(int, const char**);
} Command;

static int newCommand(int argc, const char** argv)
{
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END()
    };

    const char* const usages[] = {
        "wray new [options] <name>",
        NULL
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nCreate a new project.", NULL);

    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1) {
        argparse_usage(&argparse);
        return 1;
    }

    mkdir(argv[0]);
    SaveFileText(TextFormat("%s/main.wren", argv[0]), "System.print(\"Hello, World!\")");

    printf("Created project %s\n", argv[0]);

    return 0;
}

static int nestCommand(int argc, const char** argv)
{
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END()
    };

    const char* const usages[] = {
        "wray nest [options] <directory>",
        NULL
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nPackage a project into a single `.egg` file.", NULL);

    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1) {
        argparse_usage(&argparse);
        return 1;
    }

    if (!DirectoryExists(argv[0])) {
        printf("Directory %s does not exist.\n", argv[0]);
        return 1;
    }

    if (!FileExists(TextFormat("%s/main.wren", argv[0]))) {
        printf("No main.wren file found in %s\n", argv[0]);
        return 1;
    }

    const char* dir = argv[0];

    const char* output = NULL;

    char lastChar = dir[TextLength(dir) - 1];
    if (lastChar == '/' || lastChar == '\\') {
        output = TextFormat("%s.egg", GetFileName(TextSubtext(dir, 0, TextLength(dir) - 1)));
    } else {
        output = TextFormat("%s.egg", GetFileName(dir));
    }

    struct zip_t* zip = zip_open(output, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

    FilePathList files = LoadDirectoryFilesEx(dir, NULL, true);

    for (int i = 0; i < (int)files.count; i++) {
        const char* name = TextSubtext(files.paths[i], TextLength(dir) + 1, TextLength(files.paths[i]) - TextLength(dir) - 1);

        zip_entry_open(zip, name);

        int size;
        unsigned char* data = LoadFileData(files.paths[i], &size);
        zip_entry_write(zip, data, size);
        UnloadFileData(data);

        zip_entry_close(zip);
    }

    UnloadDirectoryFiles(files);

    zip_close(zip);

    printf("Packaged %s as %s\n", argv[0], output);

    return 0;
}

static unsigned char* checkFused(const char* selfPath, int* size)
{
    FILE* self = fopen(selfPath, "rb");
    if (self == NULL) {
        printf("Failed to open self.\n");
        return NULL;
    }

    int s;
    char magic[5];

    fseek(self, -8, SEEK_END);

    fread(&s, sizeof(int), 1, self);
    fread(magic, sizeof(char), 4, self);
    magic[4] = '\0';

    if (TextIsEqual(magic, "WRAY")) {
        unsigned char* buffer = (unsigned char*)malloc(s);
        if (buffer == NULL) {
            printf("Failed to allocate buffer.\n");
            fclose(self);
            return NULL;
        }

        fseek(self, -8 - s, SEEK_END);
        fread(buffer, s, 1, self);

        fclose(self);

        *size = s;

        return buffer;
    }

    return NULL;
}

static void fuse(const char* selfPath, const char* eggPath)
{
    int selfSize;
    unsigned char* self = LoadFileData(selfPath, &selfSize);

    int eggSize;
    unsigned char* egg = LoadFileData(eggPath, &eggSize);

    unsigned char* output = (unsigned char*)malloc(selfSize + eggSize + 8);
    memcpy(output, self, selfSize);
    memcpy(output + selfSize, egg, eggSize);

    memcpy(output + selfSize + eggSize, &eggSize, sizeof(int));
    memcpy(output + selfSize + eggSize + 4, "WRAY", 4);

#ifdef _WIN32
    const char* outName = TextFormat("%s.exe", GetFileNameWithoutExt(eggPath));
#else
    const char* outName = TextFormat("%s_out", GetFileNameWithoutExt(eggPath));
#endif

    SaveFileData(outName, output, selfSize + eggSize + 8);

#ifndef _WIN32
    chmod(outName, 0777);
#endif
}

static int fuseCommand(int argc, const char** argv)
{
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_END()
    };

    const char* const usages[] = {
        "wray fuse [options] <egg>",
        NULL
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nCreate a single executable from a `.egg` file.", NULL);

    argc = argparse_parse(&argparse, argc, argv);
    if (argc < 1) {
        argparse_usage(&argparse);
        return 1;
    }

    if (!FileExists(argv[0])) {
        printf("File %s does not exist.\n", argv[0]);
        return 1;
    }

    fuse(selfPath, argv[0]);

#ifdef _WIN32
    printf("Created %s\n", TextFormat("%s.exe", GetFileNameWithoutExt(argv[0])));
#else
    printf("Created %s\n", TextFormat("%s_out", GetFileNameWithoutExt(argv[0])));
#endif

    return 0;
}

static Command commands[] = {
    { "new", newCommand },
    { "nest", nestCommand },
    { "fuse", fuseCommand }
};

static int versionCallback(struct argparse* self, const struct argparse_option* option)
{
    printf("Wray %s\nWren %s\n", WRAY_VERSION, WREN_VERSION_STRING);
    exit(0);
}

int main(int argc, char** argv)
{
    SetTraceLogLevel(LOG_NONE);

    int size;
    unsigned char* data = checkFused(argv[0], &size);
    if (data != NULL) {
        setArgs(argc, argv);

        egg = zip_stream_open(data, size, 0, 'r');

        SetLoadFileDataCallback(zipLoadFileData);
        SetLoadFileTextCallback(zipLoadFileText);

        runWren("main.wren", "main");

        zip_close(egg);

        return 0;
    }

    TextCopy(selfPath, argv[0]);

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_BOOLEAN('v', "version", NULL, "show the version number and exit", versionCallback, 0, OPT_NONEG),
        OPT_END()
    };

    const char* const usages[] = {
        "wray [options] <file|directory|egg> [args]",
        "wray [options] <command>",
        NULL
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, ARGPARSE_STOP_AT_NON_OPTION);
    argparse_describe(&argparse, "\nAvailable commands: new, nest, fuse. Use `wray <command> --help` for details.", NULL);

    argc = argparse_parse(&argparse, argc, (const char**)argv);
    if (argc < 1) {
        argparse_usage(&argparse);
        return 1;
    }

    Command* command = NULL;
    for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
        if (TextIsEqual(commands[i].name, argv[0])) {
            command = &commands[i];
        }
    }

    if (command) {
        return command->fn(argc, (const char**)argv);
    }

    setArgs(argc, argv);

    if (DirectoryExists(argv[0])) {
        ChangeDirectory(argv[0]);

        if (!FileExists("main.wren")) {
            printf("No main.wren file found in %s\n", argv[0]);
            return 1;
        }

        runWren("main.wren", "main");
    } else if (FileExists(argv[0]) && TextIsEqual(GetFileExtension(argv[0]), ".egg")) {
        egg = zip_open(argv[0], 0, 'r');

        SetLoadFileDataCallback(zipLoadFileData);
        SetLoadFileTextCallback(zipLoadFileText);

        runWren("main.wren", "main");

        zip_close(egg);
    } else if (FileExists(argv[0])) {
        if (!TextIsEqual(GetFileExtension(argv[0]), ".wren")) {
            printf("%s is not a wren source file.\n", argv[0]);
            return 1;
        }

        ChangeDirectory(GetDirectoryPath(argv[0]));

        runWren(GetFileName(argv[0]), GetFileNameWithoutExt(argv[0]));
    } else {
        printf("No such file or directory: %s\n", argv[0]);
        return 1;
    }

    return 0;
}
