#include "cli.h"

int main(int argc, char **argv) {
    ExecResult result = runCli(argc, argv);
    if (result != EXEC_OK) {
        return 1;
    }

    return 0;
}