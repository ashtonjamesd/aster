#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cli.h"
#include "compile.h"

static inline char *readFile(char *path) {
    FILE *fptr = fopen(path, "rb");
    if (!fptr) {
        fprintf(stderr, "error opening source file\n");
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    long sz = ftell(fptr);
    rewind(fptr);

    char *buff = malloc(sz + 1);
    if (!buff) {
        fprintf(stderr, "allocation failed at %s:%d\n", __FILE__, __LINE__);
        return NULL;
    }

    fread(buff, 1, sz, fptr);
    buff[sz] = '\0';

    fclose(fptr);

    return buff;
}

ExecResult runFromSource(char *path) {
    char *source = readFile(path);
    if (!source) return EXEC_FAIL;

    AsterConfig config = {
        .lexerDebug = false,
        .parserDebug = true,
        .path = path
    };

    AsterCompiler aster = newCompiler(source, config);
    ExecResult result = compileToC(&aster);

    return result;
}

ExecResult runRepl() {
    fprintf(stderr, "REPL is currently not supported.");
    return EXEC_OK;
}

ExecResult runCli(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "must specify an execution mode.\n");
        return EXEC_FAIL;
    }

    bool isRepl = false;
    bool isFile = false;
    char *path = NULL;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--repl") == 0) {
            isRepl = true;
        } else if (strcmp(argv[i], "--path") == 0) {
            isFile = true;

            if (i + 1 >= argc) {
                fprintf(stderr, "expected argument after '--path'\n");
                return EXEC_FAIL;
            } else {
                path = argv[++i];
            }
        }
    }

    if (isFile && isRepl) {
        fprintf(stderr, "invalid flag combination, '--repl' and '--path'\n");
        return EXEC_FAIL;
    }

    if (!isFile && !isRepl) {
        fprintf(stderr, "must specify execution mode.\n");
        return EXEC_FAIL;
    }

    if (isFile) {
        return runFromSource(path);
    } else if (isRepl) {
        return runRepl();
    }

    fprintf(stderr, "unable to find a valid execution mode.\n");
    return EXEC_FAIL;
}