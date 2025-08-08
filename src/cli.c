#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

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

void runC() {
    int code = system("gcc out.c -o out && ./out");
    if (code != 0) {
        fprintf(stderr, "compilation or execution failed\n");
    }

    system("rm out");
    system("rm out.c");
}


ExecResult runFromSource(char *path) {
    char *source = readFile(path);
    if (!source) return EXEC_FAIL;

    AsterConfig config = {
        .lexerDebug = true,
        .parserDebug = true,
        .path = path
    };

    AsterCompiler aster = newCompiler(source, config);
    ExecResult result = compileToC(&aster);

    runC();

    return EXEC_OK;
}

ExecResult runRepl() {
    fprintf(stderr, "REPL is currently not supported.");

    return EXEC_OK;
}

ExecResult runCreate(char *projectName) {
    int status = mkdir("srcc", 0755);
    if (status != 0 && errno != EEXIST) {
        perror("error creating 'src' directory");
        return EXEC_FAIL;
    }

    FILE *mainFptr = fopen("srcc/main.ast", "w");
    if (!mainFptr) {
        fprintf(stderr, "error creating 'main.ast'\n");
        return EXEC_FAIL;
    }
    fprintf(mainFptr, "// this is the entry point to your application\n");
    fprintf(mainFptr, "pub fn main(argc: i32, argv: **i8): i32 {\n");
    fprintf(mainFptr, "\t// ..\n\n");
    fprintf(mainFptr, "\treturn 0\n");
    fprintf(mainFptr, "}");
    fclose(mainFptr);

    FILE *yamlFptr = fopen("aster.yaml", "w");
    if (!yamlFptr) {
        fprintf(stderr, "error creating 'aster.yaml'\n");
        return EXEC_FAIL;
    }
    fprintf(yamlFptr, "project:\n");
    fprintf(yamlFptr, "\tname: %s", projectName);

    fclose(yamlFptr);

    return EXEC_OK;
}

ExecResult runProject() {
    FILE *fptr = fopen("aster.yaml", "r");
    if (!fptr) {
        fprintf(stderr, "unable to find 'aster.yaml'\n");
        return EXEC_FAIL;
    }
    fclose(fptr);

    char *source = readFile("srcc/main.ast");
    if (!source) return EXEC_FAIL;

    AsterConfig config = {
        .lexerDebug = false,
        .parserDebug = false,
        .path = "srcc/main.ast"
    };

    AsterCompiler aster = newCompiler(source, config);
    ExecResult result = compileToC(&aster);

    return result;
}

ExecResult runCli(int argc, char **argv) {
    bool isRepl = false;
    bool isFile = false;
    char *path = NULL;

    if (argc < 2) {
        fprintf(stderr, "usage: aster <command>\n");
        return EXEC_FAIL;
    }

    if (strcmp(argv[1], "create") == 0) {
        if (argc < 3) {
            fprintf(stderr, "usage: aster create <project_name>\n");
            return EXEC_FAIL;
        }

        return runCreate(argv[2]);
    } else if (strcmp(argv[1], "run") == 0) {
        return runProject();
    }

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