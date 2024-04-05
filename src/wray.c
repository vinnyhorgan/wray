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
        if (TextIsEqual(signature, "closed"))
            return windowGetClosed;
        if (TextIsEqual(signature, "width"))
            return windowGetWidth;
        if (TextIsEqual(signature, "height"))
            return windowGetHeight;
        if (TextIsEqual(signature, "fps"))
            return windowGetFps;
        if (TextIsEqual(signature, "targetFps=(_)"))
            return windowSetTargetFps;
        if (TextIsEqual(signature, "resizable=(_)"))
            return windowSetResizable;
    } else if (TextIsEqual(className, "Graphics")) {
        if (TextIsEqual(signature, "begin()"))
            return graphicsBegin;
        if (TextIsEqual(signature, "end()"))
            return graphicsEnd;
        if (TextIsEqual(signature, "clear(_)"))
            return graphicsClear;
        if (TextIsEqual(signature, "print(_,_,_,_,_)"))
            return graphicsPrint;
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
    } else if (TextIsEqual(className, "Mouse")) {
        if (TextIsEqual(signature, "pressed(_)"))
            return mousePressed;
        if (TextIsEqual(signature, "down(_)"))
            return mouseDown;
        if (TextIsEqual(signature, "x"))
            return mouseGetX;
        if (TextIsEqual(signature, "y"))
            return mouseGetY;
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
    } else if (TextIsEqual(className, "Sound")) {
        methods.allocate = soundAllocate;
        methods.finalize = soundFinalize;
    } else if (TextIsEqual(className, "Font")) {
        methods.allocate = fontAllocate;
        methods.finalize = fontFinalize;
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

    printf("Not implemented yet.\n");

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
