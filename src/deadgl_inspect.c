#include "deadgl.h"
#include <stdio.h>
#include <string.h>

int dgl_inspect_file(const char *path);

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("DEADGL_INSPECT %s\n", dgl_version());
        return 0;
    }
    if (argc == 2) {
        return dgl_inspect_file(argv[1]);
    }
    puts("DEADGL_INSPECT");
    puts("usage:");
    puts("  deadgl-inspect --version");
    puts("  deadgl-inspect scene.dgl");
    return argc == 1 ? 0 : 1;
}
