#include "deadgl.h"

#include <stdio.h>
#include <string.h>

#define DGB_MAGIC "DEADGL_DGB_1\n"
#define SHELL_LINE_MAX 1024

int deadgl_render_main(int argc, char **argv);
int dgl_inspect_file(const char *path);

static int copy_bytes(FILE *in, FILE *out) {
    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1u, sizeof(buf), in)) > 0u) {
        if (fwrite(buf, 1u, n, out) != n) { return 1; }
    }
    return ferror(in) ? 1 : 0;
}

static int count_payload(FILE *in, unsigned long *bytes, unsigned long *lines) {
    int ch;
    *bytes = 0u;
    *lines = 0u;
    while ((ch = fgetc(in)) != EOF) {
        (*bytes)++;
        if (ch == '\n') { (*lines)++; }
    }
    return ferror(in) ? 1 : 0;
}

static int pack_dgb(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    FILE *out;
    if (in == NULL) { fprintf(stderr, "deadgl: cannot open %s\n", src); return 1; }
    out = fopen(dst, "wb");
    if (out == NULL) { fprintf(stderr, "deadgl: cannot write %s\n", dst); fclose(in); return 1; }
    if (fputs(DGB_MAGIC, out) < 0 || copy_bytes(in, out) != 0) { fprintf(stderr, "deadgl: pack failed\n"); fclose(in); fclose(out); return 1; }
    fclose(in);
    return fclose(out) == 0 ? 0 : 1;
}

static int read_dgb_header(FILE *in, const char *src) {
    char magic[sizeof(DGB_MAGIC)];
    if (fread(magic, 1u, sizeof(DGB_MAGIC) - 1u, in) != sizeof(DGB_MAGIC) - 1u) { fprintf(stderr, "deadgl: bad dgb header %s\n", src); return 1; }
    magic[sizeof(DGB_MAGIC) - 1u] = '\0';
    if (strcmp(magic, DGB_MAGIC) != 0) { fprintf(stderr, "deadgl: bad dgb magic %s\n", src); return 1; }
    return 0;
}

static int unpack_dgb(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    FILE *out;
    if (in == NULL) { fprintf(stderr, "deadgl: cannot open %s\n", src); return 1; }
    if (read_dgb_header(in, src) != 0) { fclose(in); return 1; }
    out = fopen(dst, "wb");
    if (out == NULL) { fprintf(stderr, "deadgl: cannot write %s\n", dst); fclose(in); return 1; }
    if (copy_bytes(in, out) != 0) { fprintf(stderr, "deadgl: unpack failed\n"); fclose(in); fclose(out); return 1; }
    fclose(in);
    return fclose(out) == 0 ? 0 : 1;
}

static int disasm_dgb(const char *src) {
    FILE *in = fopen(src, "rb");
    unsigned long bytes;
    unsigned long lines;
    if (in == NULL) { fprintf(stderr, "deadgl: cannot open %s\n", src); return 1; }
    if (read_dgb_header(in, src) != 0) { fclose(in); return 1; }
    if (count_payload(in, &bytes, &lines) != 0) { fprintf(stderr, "deadgl: disasm failed\n"); fclose(in); return 1; }
    fclose(in);
    printf("DEADGL_DGB\n");
    printf("version %s\n", dgl_version());
    printf("file %s\n", src);
    printf("magic DEADGL_DGB_1\n");
    printf("payload_bytes %lu\n", bytes);
    printf("payload_lines %lu\n", lines);
    return 0;
}

static int text_demo(const char *out) {
    DGL_Surface s = {0, 0, NULL, NULL};
    int rc = dgl_surface_init(&s, 640, 160);
    if (rc != DGL_OK) { fprintf(stderr, "deadgl: surface init failed: %s\n", dgl_result_name(rc)); return 1; }
    dgl_clear(&s, DGL_RGB(5, 6, 8));
    dgl_clear_depth(&s, DGL_FAR);
    dgl_text(&s, 20, 24, DGL_RGB(0, 255, 153), "DEADGL SOFTWARE CONSOLE");
    dgl_text(&s, 20, 48, DGL_RGB(240, 240, 216), "CPU WRITES PIXELS");
    dgl_text(&s, 20, 72, DGL_RGB(255, 136, 34), "NO GPU NO ENGINE");
    printf("hash %016llx\n", (unsigned long long)dgl_hash(&s));
    rc = dgl_save_ppm(&s, out);
    dgl_surface_free(&s);
    return rc == DGL_OK ? 0 : 1;
}

static int tile_demo(const char *out) {
    DGL_Surface s = {0, 0, NULL, NULL};
    int rc = dgl_surface_init(&s, 640, 360);
    if (rc != DGL_OK) { fprintf(stderr, "deadgl: surface init failed: %s\n", dgl_result_name(rc)); return 1; }
    dgl_tile_grid(&s, 32, DGL_RGB(8, 10, 16), DGL_RGB(18, 22, 32));
    dgl_tile_frame(&s, 32, DGL_RGB(0, 255, 153));
    dgl_text(&s, 32, 32, DGL_RGB(240, 240, 216), "DEADGL TILE RENDERER");
    dgl_text(&s, 32, 56, DGL_RGB(255, 136, 34), "CPU TILE WALK");
    printf("hash %016llx\n", (unsigned long long)dgl_hash(&s));
    rc = dgl_save_ppm(&s, out);
    dgl_surface_free(&s);
    return rc == DGL_OK ? 0 : 1;
}

static int run_file_shell(const char *src, const char *out) {
    char *args[5];
    args[0] = "deadgl";
    args[1] = "run";
    args[2] = (char *)src;
    args[3] = "-o";
    args[4] = (char *)out;
    return deadgl_render_main(5, args);
}

static int stdin_shell(const char *out) {
    char scene[SHELL_LINE_MAX];
    char line[SHELL_LINE_MAX];
    FILE *f;
    int rc;
    if (snprintf(scene, sizeof(scene), "%s.shell.dgl", out) < 0 || strlen(out) + 10u >= sizeof(scene)) { fprintf(stderr, "deadgl: shell path too long\n"); return 1; }
    f = fopen(scene, "wb");
    if (f == NULL) { fprintf(stderr, "deadgl: cannot write shell stream %s\n", scene); return 1; }
    while (fgets(line, sizeof(line), stdin) != NULL) {
        if (fputs(line, f) < 0) { fclose(f); remove(scene); return 1; }
    }
    if (ferror(stdin)) { fclose(f); remove(scene); return 1; }
    if (fclose(f) != 0) { remove(scene); return 1; }
    rc = run_file_shell(scene, out);
    remove(scene);
    return rc;
}

int main(int argc, char **argv) {
    if (argc == 3 && (strcmp(argv[1], "inspect") == 0 || strcmp(argv[1], "audit") == 0)) { return dgl_inspect_file(argv[2]); }
    if (argc == 3 && strcmp(argv[1], "disasm") == 0) { return disasm_dgb(argv[2]); }
    if (argc == 4 && strcmp(argv[1], "textdemo") == 0 && strcmp(argv[2], "-o") == 0) { return text_demo(argv[3]); }
    if (argc == 4 && strcmp(argv[1], "tiledemo") == 0 && strcmp(argv[2], "-o") == 0) { return tile_demo(argv[3]); }
    if (argc == 4 && strcmp(argv[1], "shell") == 0 && strcmp(argv[2], "-o") == 0) { return stdin_shell(argv[3]); }
    if (argc == 5 && strcmp(argv[1], "shell") == 0 && strcmp(argv[3], "-o") == 0) { return run_file_shell(argv[2], argv[4]); }
    if (argc == 5 && strcmp(argv[1], "pack") == 0 && strcmp(argv[3], "-o") == 0) { return pack_dgb(argv[2], argv[4]); }
    if (argc == 5 && strcmp(argv[1], "unpack") == 0 && strcmp(argv[3], "-o") == 0) { return unpack_dgb(argv[2], argv[4]); }
    return deadgl_render_main(argc, argv);
}
