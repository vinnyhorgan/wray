#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "lib/argparse/argparse.h"
#include "lib/wren/wren.h"
#include "lib/zip/zip.h"

#include "api.h"
#include "api.wren.h"

#define WRAY_VERSION "0.1.0"

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

static WrenForeignMethodFn wrenBindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature)
{
    if (TextIsEqual(className, "Audio")) {
        if (TextIsEqual(signature, "init()"))
            return audioInit;
        if (TextIsEqual(signature, "close()"))
            return audioClose;
        if (TextIsEqual(signature, "volume"))
            return audioGetVolume;
        if (TextIsEqual(signature, "volume=(_)"))
            return audioSetVolume;
    } else if (TextIsEqual(className, "Sound")) {
        if (TextIsEqual(signature, "init new(_)"))
            return soundNew;
        if (TextIsEqual(signature, "play()"))
            return soundPlay;
        if (TextIsEqual(signature, "stop()"))
            return soundStop;
        if (TextIsEqual(signature, "pause()"))
            return soundPause;
        if (TextIsEqual(signature, "resume()"))
            return soundResume;
        if (TextIsEqual(signature, "playing"))
            return soundGetPlaying;
        if (TextIsEqual(signature, "volume=(_)"))
            return soundSetVolume;
        if (TextIsEqual(signature, "pitch=(_)"))
            return soundSetPitch;
        if (TextIsEqual(signature, "pan=(_)"))
            return soundSetPan;
    } else if (TextIsEqual(className, "Font")) {
        if (TextIsEqual(signature, "init new(_,_)"))
            return fontNew;
        if (TextIsEqual(signature, "print(_,_,_,_,_,_,_)"))
            return fontPrint;
        if (TextIsEqual(signature, "measure(_)"))
            return fontMeasure;
        if (TextIsEqual(signature, "size"))
            return fontGetSize;
    } else if (TextIsEqual(className, "Window")) {
        if (TextIsEqual(signature, "init(_,_,_)"))
            return windowInit;
        if (TextIsEqual(signature, "close()"))
            return windowClose;
        if (TextIsEqual(signature, "toggleFullscreen()"))
            return windowToggleFullscreen;
        if (TextIsEqual(signature, "maximize()"))
            return windowMaximize;
        if (TextIsEqual(signature, "minimize()"))
            return windowMinimize;
        if (TextIsEqual(signature, "restore()"))
            return windowRestore;
        if (TextIsEqual(signature, "setPosition(_,_)"))
            return windowSetPosition;
        if (TextIsEqual(signature, "setMinSize(_,_)"))
            return windowSetMinSize;
        if (TextIsEqual(signature, "setMaxSize(_,_)"))
            return windowSetMaxSize;
        if (TextIsEqual(signature, "setSize(_,_)"))
            return windowSetSize;
        if (TextIsEqual(signature, "focus()"))
            return windowFocus;
        if (TextIsEqual(signature, "closed"))
            return windowGetClosed;
        if (TextIsEqual(signature, "fullscreen"))
            return windowGetFullscreen;
        if (TextIsEqual(signature, "hidden"))
            return windowGetHidden;
        if (TextIsEqual(signature, "minimized"))
            return windowGetMinimized;
        if (TextIsEqual(signature, "maximized"))
            return windowGetMaximized;
        if (TextIsEqual(signature, "focused"))
            return windowGetFocused;
        if (TextIsEqual(signature, "resized"))
            return windowGetResized;
        if (TextIsEqual(signature, "title=(_)"))
            return windowSetTitle;
        if (TextIsEqual(signature, "width"))
            return windowGetWidth;
        if (TextIsEqual(signature, "height"))
            return windowGetHeight;
        if (TextIsEqual(signature, "x"))
            return windowGetX;
        if (TextIsEqual(signature, "y"))
            return windowGetY;
        if (TextIsEqual(signature, "dpi"))
            return windowGetDpi;
        if (TextIsEqual(signature, "resizable"))
            return windowGetResizable;
        if (TextIsEqual(signature, "resizable=(_)"))
            return windowSetResizable;
        if (TextIsEqual(signature, "vsync"))
            return windowGetVSync;
        if (TextIsEqual(signature, "vsync=(_)"))
            return windowSetVSync;
        if (TextIsEqual(signature, "targetFps=(_)"))
            return windowSetTargetFps;
        if (TextIsEqual(signature, "dt"))
            return windowGetDt;
        if (TextIsEqual(signature, "time"))
            return windowGetTime;
        if (TextIsEqual(signature, "fps"))
            return windowGetFps;
    } else if (TextIsEqual(className, "Graphics")) {
        if (TextIsEqual(signature, "begin()"))
            return graphicsBegin;
        if (TextIsEqual(signature, "end()"))
            return graphicsEnd;
        if (TextIsEqual(signature, "beginBlend(_)"))
            return graphicsBeginBlend;
        if (TextIsEqual(signature, "endBlend()"))
            return graphicsEndBlend;
        if (TextIsEqual(signature, "beginScissor(_,_,_,_)"))
            return graphicsBeginScissor;
        if (TextIsEqual(signature, "endScissor()"))
            return graphicsEndScissor;
        if (TextIsEqual(signature, "clear(_)"))
            return graphicsClear;
        if (TextIsEqual(signature, "print(_,_,_,_,_)"))
            return graphicsPrint;
        if (TextIsEqual(signature, "takeScreenshot(_)"))
            return graphicsTakeScreenshot;
        if (TextIsEqual(signature, "pixel(_,_,_)"))
            return graphicsPixel;
        if (TextIsEqual(signature, "line(_,_,_,_,_,_)"))
            return graphicsLine;
        if (TextIsEqual(signature, "circle(_,_,_,_)"))
            return graphicsCircle;
        if (TextIsEqual(signature, "circleLine(_,_,_,_)"))
            return graphicsCircleLine;
        if (TextIsEqual(signature, "ellipse(_,_,_,_,_)"))
            return graphicsEllipse;
        if (TextIsEqual(signature, "ellipseLine(_,_,_,_,_)"))
            return graphicsEllipseLine;
        if (TextIsEqual(signature, "rectangle(_,_,_,_,_,_,_,_)"))
            return graphicsRectangle;
        if (TextIsEqual(signature, "rectangleLine(_,_,_,_,_,_)"))
            return graphicsRectangleLine;
        if (TextIsEqual(signature, "triangle(_,_,_,_,_,_,_)"))
            return graphicsTriangle;
        if (TextIsEqual(signature, "triangleLine(_,_,_,_,_,_,_)"))
            return graphicsTriangleLine;
        if (TextIsEqual(signature, "polygon(_,_,_,_,_,_)"))
            return graphicsPolygon;
        if (TextIsEqual(signature, "polygonLine(_,_,_,_,_,_,_)"))
            return graphicsPolygonLine;
        if (TextIsEqual(signature, "noise(_,_,_,_)"))
            return graphicsNoise;
        if (TextIsEqual(signature, "noiseSeed=(_)"))
            return graphicsSetNoiseSeed;
    } else if (TextIsEqual(className, "Mouse")) {
        if (TextIsEqual(signature, "down(_)"))
            return mouseDown;
        if (TextIsEqual(signature, "pressed(_)"))
            return mousePressed;
        if (TextIsEqual(signature, "released(_)"))
            return mouseReleased;
        if (TextIsEqual(signature, "setPosition(_,_)"))
            return mouseSetPosition;
        if (TextIsEqual(signature, "setOffset(_,_)"))
            return mouseSetOffset;
        if (TextIsEqual(signature, "setScale(_,_)"))
            return mouseSetScale;
        if (TextIsEqual(signature, "x"))
            return mouseGetX;
        if (TextIsEqual(signature, "y"))
            return mouseGetY;
        if (TextIsEqual(signature, "dx"))
            return mouseGetDx;
        if (TextIsEqual(signature, "dy"))
            return mouseGetDy;
        if (TextIsEqual(signature, "wheel"))
            return mouseGetWheel;
        if (TextIsEqual(signature, "cursor=(_)"))
            return mouseSetCursor;
        if (TextIsEqual(signature, "hidden"))
            return mouseGetHidden;
        if (TextIsEqual(signature, "hidden=(_)"))
            return mouseSetHidden;
        if (TextIsEqual(signature, "enabled=(_)"))
            return mouseSetEnabled;
        if (TextIsEqual(signature, "onScreen"))
            return mouseGetOnScreen;
    } else if (TextIsEqual(className, "Keyboard")) {
        if (TextIsEqual(signature, "down(_)"))
            return keyboardDown;
        if (TextIsEqual(signature, "pressed(_)"))
            return keyboardPressed;
        if (TextIsEqual(signature, "pressedRepeat(_)"))
            return keyboardPressedRepeat;
        if (TextIsEqual(signature, "released(_)"))
            return keyboardReleased;
        if (TextIsEqual(signature, "keyPressed"))
            return keyboardGetKeyPressed;
    } else if (TextIsEqual(className, "Color")) {
        if (TextIsEqual(signature, "init new(_,_,_,_)"))
            return colorNew;
        if (TextIsEqual(signature, "init new(_,_,_)"))
            return colorNew2;
        if (TextIsEqual(signature, "[_]"))
            return colorGetIndex;
        if (TextIsEqual(signature, "[_]=(_)"))
            return colorSetIndex;
    } else if (TextIsEqual(className, "Texture")) {
        if (TextIsEqual(signature, "init new(_)"))
            return textureNew;
        if (TextIsEqual(signature, "draw(_,_,_,_,_,_,_,_)"))
            return textureDraw;
        if (TextIsEqual(signature, "drawRect(_,_,_,_,_,_,_,_,_,_,_,_)"))
            return textureDrawRect;
        if (TextIsEqual(signature, "width"))
            return textureGetWidth;
        if (TextIsEqual(signature, "height"))
            return textureGetHeight;
        if (TextIsEqual(signature, "filter=(_)"))
            return textureSetFilter;
        if (TextIsEqual(signature, "wrap=(_)"))
            return textureSetWrap;
    } else if (TextIsEqual(className, "RenderTexture")) {
        if (TextIsEqual(signature, "init new(_,_)"))
            return renderTextureNew;
        if (TextIsEqual(signature, "begin()"))
            return renderTextureBegin;
        if (TextIsEqual(signature, "end()"))
            return renderTextureEnd;
        if (TextIsEqual(signature, "draw(_,_,_,_,_,_,_,_)"))
            return renderTextureDraw;
        if (TextIsEqual(signature, "drawRect(_,_,_,_,_,_,_,_,_,_,_,_)"))
            return renderTextureDrawRect;
        if (TextIsEqual(signature, "width"))
            return renderTextureGetWidth;
        if (TextIsEqual(signature, "height"))
            return renderTextureGetHeight;
        if (TextIsEqual(signature, "filter=(_)"))
            return renderTextureSetFilter;
        if (TextIsEqual(signature, "wrap=(_)"))
            return renderTextureSetWrap;
    } else if (TextIsEqual(className, "Gamepad")) {
        if (TextIsEqual(signature, "init new(_)"))
            return gamepadNew;
        if (TextIsEqual(signature, "available(_)"))
            return gamepadAvailable;
        if (TextIsEqual(signature, "down(_)"))
            return gamepadDown;
        if (TextIsEqual(signature, "pressed(_)"))
            return gamepadPressed;
        if (TextIsEqual(signature, "released(_)"))
            return gamepadReleased;
        if (TextIsEqual(signature, "axis(_)"))
            return gamepadAxis;
        if (TextIsEqual(signature, "id"))
            return gamepadGetId;
        if (TextIsEqual(signature, "name"))
            return gamepadGetName;
        if (TextIsEqual(signature, "axisCount"))
            return gamepadGetAxisCount;
    } else if (TextIsEqual(className, "Request")) {
        if (TextIsEqual(signature, "init new(_)"))
            return requestNew;
        if (TextIsEqual(signature, "make()"))
            return requestMake;
        if (TextIsEqual(signature, "complete"))
            return requestGetComplete;
        if (TextIsEqual(signature, "status"))
            return requestGetStatus;
        if (TextIsEqual(signature, "body"))
            return requestGetBody;
    } else if (TextIsEqual(className, "Camera")) {
        if (TextIsEqual(signature, "init new(_,_)"))
            return cameraNew;
        if (TextIsEqual(signature, "begin()"))
            return cameraBegin;
        if (TextIsEqual(signature, "end()"))
            return cameraEnd;
        if (TextIsEqual(signature, "x"))
            return cameraGetX;
        if (TextIsEqual(signature, "y"))
            return cameraGetY;
        if (TextIsEqual(signature, "ox"))
            return cameraGetOffsetX;
        if (TextIsEqual(signature, "oy"))
            return cameraGetOffsetY;
        if (TextIsEqual(signature, "r"))
            return cameraGetRotation;
        if (TextIsEqual(signature, "zoom"))
            return cameraGetZoom;
        if (TextIsEqual(signature, "x=(_)"))
            return cameraSetX;
        if (TextIsEqual(signature, "y=(_)"))
            return cameraSetY;
        if (TextIsEqual(signature, "ox=(_)"))
            return cameraSetOffsetX;
        if (TextIsEqual(signature, "oy=(_)"))
            return cameraSetOffsetY;
        if (TextIsEqual(signature, "r=(_)"))
            return cameraSetRotation;
        if (TextIsEqual(signature, "zoom=(_)"))
            return cameraSetZoom;
    } else if (TextIsEqual(className, "Shader")) {
        if (TextIsEqual(signature, "init new(_)"))
            return shaderNew;
        if (TextIsEqual(signature, "init new(_,_)"))
            return shaderNew2;
        if (TextIsEqual(signature, "begin()"))
            return shaderBegin;
        if (TextIsEqual(signature, "end()"))
            return shaderEnd;
        if (TextIsEqual(signature, "set(_,_)"))
            return shaderSet;
    } else if (TextIsEqual(className, "OS")) {
        if (TextIsEqual(signature, "openUrl(_)"))
            return osOpenUrl;
        if (TextIsEqual(signature, "readLine()"))
            return osReadLine;
        if (TextIsEqual(signature, "args"))
            return osGetArgs;
        if (TextIsEqual(signature, "name"))
            return osGetName;
        if (TextIsEqual(signature, "clipboard"))
            return osGetClipboard;
        if (TextIsEqual(signature, "clipboard=(_)"))
            return osSetClipboard;
    } else if (TextIsEqual(className, "Directory")) {
        if (TextIsEqual(signature, "exists(_)"))
            return directoryExists;
        if (TextIsEqual(signature, "list(_)"))
            return directoryList;
    } else if (TextIsEqual(className, "File")) {
        if (TextIsEqual(signature, "exists(_)"))
            return fileExists;
        if (TextIsEqual(signature, "write(_,_)"))
            return fileWrite;
        if (TextIsEqual(signature, "read(_)"))
            return fileRead;
        if (TextIsEqual(signature, "size(_)"))
            return fileSize;
    }

    return NULL;
}

static WrenForeignClassMethods wrenBindForeignClass(WrenVM* vm, const char* module, const char* className)
{
    WrenForeignClassMethods methods = { 0 };

    if (TextIsEqual(className, "Color")) {
        methods.allocate = colorAllocate;
    } else if (TextIsEqual(className, "Texture")) {
        methods.allocate = textureAllocate;
        methods.finalize = textureFinalize;
    } else if (TextIsEqual(className, "RenderTexture")) {
        methods.allocate = renderTextureAllocate;
        methods.finalize = renderTextureFinalize;
    } else if (TextIsEqual(className, "Sound")) {
        methods.allocate = soundAllocate;
        methods.finalize = soundFinalize;
    } else if (TextIsEqual(className, "Font")) {
        methods.allocate = fontAllocate;
        methods.finalize = fontFinalize;
    } else if (TextIsEqual(className, "Gamepad")) {
        methods.allocate = gamepadAllocate;
    } else if (TextIsEqual(className, "Camera")) {
        methods.allocate = cameraAllocate;
    } else if (TextIsEqual(className, "Shader")) {
        methods.allocate = shaderAllocate;
        methods.finalize = shaderFinalize;
    } else if (TextIsEqual(className, "Request")) {
        methods.allocate = requestAllocate;
        methods.finalize = requestFinalize;
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
    wrenInterpret(vm, module, source);

    UnloadFileText(source);
    wrenFreeVM(vm);
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

    printf("Created project %s.\n", argv[0]);

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
        printf("No main.wren file found in %s.\n", argv[0]);
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

    printf("Packaged %s as %s.\n", argv[0], output);

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

    SaveFileData(TextFormat("%s.exe", GetFileNameWithoutExt(eggPath)), output, selfSize + eggSize + 8);
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

    printf("Created %s.\n", TextFormat("%s.exe", GetFileNameWithoutExt(argv[0])));

    return 0;
}

static Command commands[] = {
    { "new", newCommand },
    { "nest", nestCommand },
    { "fuse", fuseCommand }
};

static int versionCallback(struct argparse* self, const struct argparse_option* option)
{
    printf("wray version %s\n", WRAY_VERSION);
    exit(0);
}

int main(int argc, char** argv)
{
    SetTraceLogLevel(LOG_NONE);

    int size;
    unsigned char* data = checkFused(argv[0], &size);
    if (data != NULL) {
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

    argc = argparse_parse(&argparse, argc, argv);
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
        return command->fn(argc, argv);
    }

    setArgs(argc, argv);

    if (DirectoryExists(argv[0])) {
        ChangeDirectory(argv[0]);

        if (!FileExists("main.wren")) {
            printf("No main.wren file found in %s.\n", argv[0]);
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
        printf("No such file or directory: %s.\n", argv[0]);
        return 1;
    }

    return 0;
}
