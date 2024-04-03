#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "lib/argparse/argparse.h"

#define WRAY_VERSION "0.1.0"

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
