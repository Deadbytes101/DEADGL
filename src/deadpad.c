#include "deadgl.h"

#include <stdio.h>
#include <string.h>

static void usage(void) {
    puts("DEADPAD");
    puts("usage:");
    puts("  deadpad --version");
    puts("  deadpad new scene.dgl");
    puts("  deadpad cat scene.dgl");
    puts("  deadpad append scene.dgl COMMAND...");
}

static int write_seed(const char *path) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) { fprintf(stderr, "deadpad: cannot write %s\n", path); return 1; }
    fputs("canvas 640 360\n", f);
    fputs("clear #050608\n", f);
    fputs("grid 32 #141820\n", f);
    fputs("line 32 120 608 120 #00ff99\n", f);
    fputs("rect 32 32 576 296 #f0f0d8\n", f);
    fputs("fillcircle 320 180 26 #ff8822\n", f);
    fputs("circle 320 180 48 #f0f0d8\n", f);
    fputs("tri 320 72 176 288 464 288 #2a96ff\n", f);
    return fclose(f) == 0 ? 0 : 1;
}

static int cat_file(const char *path) {
    FILE *f = fopen(path, "rb");
    char line[1024];
    unsigned long n = 1u;
    if (f == NULL) { fprintf(stderr, "deadpad: cannot open %s\n", path); return 1; }
    while (fgets(line, sizeof(line), f) != NULL) {
        printf("%04lu  %s", n, line);
        n++;
    }
    return fclose(f) == 0 ? 0 : 1;
}

static int append_command(int argc, char **argv) {
    FILE *f;
    int i;
    if (argc < 4) { usage(); return 1; }
    f = fopen(argv[2], "ab");
    if (f == NULL) { fprintf(stderr, "deadpad: cannot write %s\n", argv[2]); return 1; }
    for (i = 3; i < argc; i++) {
        if (i > 3 && fputc(' ', f) == EOF) { fclose(f); return 1; }
        if (fputs(argv[i], f) < 0) { fclose(f); return 1; }
    }
    if (fputc('\n', f) == EOF) { fclose(f); return 1; }
    return fclose(f) == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("DEADPAD %s\n", dgl_version());
        return 0;
    }
    if (argc == 3 && strcmp(argv[1], "new") == 0) { return write_seed(argv[2]); }
    if (argc == 3 && strcmp(argv[1], "cat") == 0) { return cat_file(argv[2]); }
    if (argc >= 4 && strcmp(argv[1], "append") == 0) { return append_command(argc, argv); }
    usage();
    return argc == 1 ? 0 : 1;
}
