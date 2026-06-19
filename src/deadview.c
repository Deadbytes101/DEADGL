#include "deadgl.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEADVIEW_MAX_DIM 16384
#define DEADVIEW_PREVIEW_W 64
#define DEADVIEW_PREVIEW_H 24

static uint64_t fnv1a_bytes(const unsigned char *data, size_t n) {
    uint64_t h = 1469598103934665603ull;
    size_t i;
    for (i = 0u; i < n; i++) {
        h ^= (uint64_t)data[i];
        h *= 1099511628211ull;
    }
    return h;
}

static int read_ppm_header(FILE *f, int *w, int *h, int *maxv) {
    char magic[3];
    int c;
    if (fscanf(f, "%2s", magic) != 1) { return 1; }
    if (strcmp(magic, "P6") != 0) { return 1; }
    if (fscanf(f, "%d %d %d", w, h, maxv) != 3) { return 1; }
    c = fgetc(f);
    if (c == EOF) { return 1; }
    if (*w <= 0 || *h <= 0 || *w > DEADVIEW_MAX_DIM || *h > DEADVIEW_MAX_DIM || *maxv != 255) { return 1; }
    return 0;
}

static unsigned char luma_at(const unsigned char *pixels, int w, int x, int y) {
    size_t p = ((size_t)y * (size_t)w + (size_t)x) * 3u;
    unsigned int r = (unsigned int)pixels[p];
    unsigned int g = (unsigned int)pixels[p + 1u];
    unsigned int b = (unsigned int)pixels[p + 2u];
    return (unsigned char)((r * 30u + g * 59u + b * 11u) / 100u);
}

static char shade(unsigned char v) {
    static const char ramp[] = " .:-=+*#%@";
    size_t idx = ((size_t)v * (sizeof(ramp) - 2u)) / 255u;
    return ramp[idx];
}

static void print_preview(const unsigned char *pixels, int w, int h) {
    int pw = w < DEADVIEW_PREVIEW_W ? w : DEADVIEW_PREVIEW_W;
    int ph = h < DEADVIEW_PREVIEW_H ? h : DEADVIEW_PREVIEW_H;
    int py;
    for (py = 0; py < ph; py++) {
        int y = (int)(((long long)py * (long long)h) / (long long)ph);
        int px;
        for (px = 0; px < pw; px++) {
            int x = (int)(((long long)px * (long long)w) / (long long)pw);
            putchar(shade(luma_at(pixels, w, x, y)));
        }
        putchar('\n');
    }
}

static int view_ppm(const char *path) {
    FILE *f = fopen(path, "rb");
    int w;
    int h;
    int maxv;
    size_t bytes;
    unsigned char *pixels;
    uint64_t hash;
    if (f == NULL) { fprintf(stderr, "deadview: cannot open %s\n", path); return 1; }
    if (read_ppm_header(f, &w, &h, &maxv) != 0) { fprintf(stderr, "deadview: bad ppm %s\n", path); fclose(f); return 1; }
    bytes = (size_t)w * (size_t)h * 3u;
    pixels = (unsigned char *)malloc(bytes);
    if (pixels == NULL) { fclose(f); return 1; }
    if (fread(pixels, 1u, bytes, f) != bytes) { fprintf(stderr, "deadview: truncated ppm %s\n", path); free(pixels); fclose(f); return 1; }
    if (fclose(f) != 0) { free(pixels); return 1; }
    hash = fnv1a_bytes(pixels, bytes);
    printf("DEADVIEW %s\n", dgl_version());
    printf("file %s\n", path);
    printf("format P6\n");
    printf("width %d\n", w);
    printf("height %d\n", h);
    printf("maxval %d\n", maxv);
    printf("payload_bytes %lu\n", (unsigned long)bytes);
    printf("pixel_hash %016llx\n", (unsigned long long)hash);
    printf("preview\n");
    print_preview(pixels, w, h);
    free(pixels);
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("DEADVIEW %s\n", dgl_version());
        return 0;
    }
    if (argc == 2) { return view_ppm(argv[1]); }
    puts("DEADVIEW");
    puts("usage:");
    puts("  deadview --version");
    puts("  deadview image.ppm");
    return argc == 1 ? 0 : 1;
}
