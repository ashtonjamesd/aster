#include "cli.h"

int main(int argc, char **argv) {
    return runCli(argc, argv) == EXEC_OK;
}