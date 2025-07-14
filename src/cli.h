#ifndef cli_h
#define cli_h

typedef enum {
    // Compilation was successful
    EXEC_OK,

    // Signifies failure pre-compilation
    EXEC_FAIL,
    
    // Signifies failure at compile time
    EXEC_COMPILE_ERR,
} ExecResult;

ExecResult runCli(int argc, char **argv);

#endif