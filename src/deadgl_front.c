#include "deadgl.h"

#include <stdio.h>
#include <string.h>

#define DGB_MAGIC "DEADGL_DGB_1\n"

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

static int unpack_dgb(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    FILE *out;
    char magic[sizeof(DGB_MAGIC)];
    if (in == NULL) { fprintf(stderr, "deadgl: cannot open %s\n", src); return 1; }
    if (fread(magic, 1u, sizeof(DGB_MAGIC) - 1u, in) != sizeof(DGB_MAGIC) - 1u) { fprintf(stderr, "deadgl: bad dgb header\n"); fclose(in); return 1; }
    magic[sizeof(DGB_MAGIC) - 1u] = '\0';
    if (strcmp(magic, DGB_MAGIC) != 0) { fprintf(stderr, "deadgl: bad dgb magic\n"); fclose(in); return 1; }
    out = fopen(dst, "wb");
    if (out == NULL) { fprintf(stderr, "deadgl: cannot write %s\n", dst); fclose(in); return 1; }
    if (copy_bytes(in, out) != 0) { fprintf(stderr, "deadgl: unpack failed\n"); fclose(in); fclose(out); return 1; }
    fclose(in);
    return fclose(out) == 0 ? 0 : 1;
}

int main(int argc, char **argv) {
    if (argc == 3 && (strcmp(argv[1], "inspect") == 0 || strcmp(argv[1], "audit") == 0)) { return dgl_inspect_file(argv[2]); }
    if (argc == 5 && strcmp(argv[1], "pack") == 0 && strcmp(argv[3], "-o") == 0) { return pack_dgb(argv[2], argv[4]); }
    if (argc == 5 && strcmp(argv[1], "unpack") == 0 && strcmp(argv[3], "-o") == 0) { return unpack_dgb(argv[2], argv[4]); }
    return deadgl_render_main(argc, argv);
}
