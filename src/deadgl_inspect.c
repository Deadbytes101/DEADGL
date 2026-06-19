#include "deadgl.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define LINE_MAX_DGL 1024

typedef struct InspectStats {
    unsigned long lines;
    unsigned long commands;
    unsigned long canvas;
    unsigned long camera;
    unsigned long near_cmd;
    unsigned long light;
    unsigned long shade;
    unsigned long cull;
    unsigned long vertex;
    unsigned long mesh;
    unsigned long primitive;
    unsigned long unknown;
} InspectStats;

static char *skip_ws(char *p) {
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    return p;
}

static int blank_or_comment(const char *p) {
    while (*p != '\0' && isspace((unsigned char)*p)) {
        p++;
    }
    return *p == '\0' || *p == '\n' || *p == '#' || *p == ';' || (p[0] == '/' && p[1] == '/');
}

static void count_command(InspectStats *s, const char *cmd) {
    s->commands++;
    if (strcmp(cmd, "canvas") == 0) { s->canvas++; return; }
    if (strcmp(cmd, "camera") == 0) { s->camera++; return; }
    if (strcmp(cmd, "near") == 0) { s->near_cmd++; return; }
    if (strcmp(cmd, "light") == 0) { s->light++; return; }
    if (strcmp(cmd, "shade") == 0) { s->shade++; return; }
    if (strcmp(cmd, "cull") == 0) { s->cull++; return; }
    if (strcmp(cmd, "vertex") == 0) { s->vertex++; return; }
    if (strncmp(cmd, "mesh", 4u) == 0) { s->mesh++; return; }
    if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "grid") == 0 || strcmp(cmd, "line") == 0 || strcmp(cmd, "rect") == 0 || strcmp(cmd, "fillrect") == 0 || strcmp(cmd, "circle") == 0 || strcmp(cmd, "fillcircle") == 0 || strcmp(cmd, "tri") == 0 || strcmp(cmd, "wiretri") == 0 || strcmp(cmd, "ztri") == 0 || strcmp(cmd, "face") == 0 || strcmp(cmd, "wireface") == 0 || strcmp(cmd, "quad") == 0 || strcmp(cmd, "wirequad") == 0) {
        s->primitive++;
        return;
    }
    s->unknown++;
}

static int inspect_file(const char *path) {
    FILE *f = fopen(path, "r");
    InspectStats s;
    char line[LINE_MAX_DGL];
    memset(&s, 0, sizeof(s));
    if (f == NULL) {
        fprintf(stderr, "deadgl-inspect: cannot open %s\n", path);
        return 1;
    }
    while (fgets(line, sizeof(line), f) != NULL) {
        char *p;
        char *cmd;
        s.lines++;
        if (blank_or_comment(line)) {
            continue;
        }
        if (strchr(line, '\n') == NULL && !feof(f)) {
            fprintf(stderr, "deadgl-inspect: %s:%lu: line too long\n", path, s.lines);
            fclose(f);
            return 1;
        }
        p = skip_ws(line);
        cmd = strtok(p, " \t\r\n");
        if (cmd != NULL) {
            count_command(&s, cmd);
        }
    }
    fclose(f);
    printf("DEADGL_INSPECT\n");
    printf("version %s\n", dgl_version());
    printf("scene %s\n", path);
    printf("lines %lu\n", s.lines);
    printf("commands %lu\n", s.commands);
    printf("canvas %lu\n", s.canvas);
    printf("camera %lu\n", s.camera);
    printf("near %lu\n", s.near_cmd);
    printf("light %lu\n", s.light);
    printf("shade %lu\n", s.shade);
    printf("cull %lu\n", s.cull);
    printf("vertex %lu\n", s.vertex);
    printf("mesh %lu\n", s.mesh);
    printf("primitive %lu\n", s.primitive);
    printf("unknown %lu\n", s.unknown);
    return s.unknown == 0u ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("DEADGL_INSPECT %s\n", dgl_version());
        return 0;
    }
    if (argc == 2) {
        return inspect_file(argv[1]);
    }
    puts("DEADGL_INSPECT");
    puts("usage:");
    puts("  deadgl-inspect --version");
    puts("  deadgl-inspect scene.dgl");
    return argc == 1 ? 0 : 1;
}
