#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "lib/argparse/argparse.h"
#include "lib/zip/zip.h"

#define WRAY_VERSION "0.1.0"

#ifdef _WIN32
#include <direct.h>
#define mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define mkdir(path) mkdir(path, 0777)
#endif

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
    SaveFileText(TextFormat("%s/main.wren", argv[0]), "System.print(\"Hello, World!\");");

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

    printf("Packaged %s as %s\n", argv[0], output);

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

    return 0;
}
