#include "deadgl.h"

#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define DGL_OUT_LEFT 1
#define DGL_OUT_RIGHT 2
#define DGL_OUT_BOTTOM 4
#define DGL_OUT_TOP 8

static int dgl_inside(const DGL_Surface *s, int x, int y) {
    return dgl_surface_valid(s) && x >= 0 && y >= 0 && x < s->width && y < s->height;
}

static size_t dgl_index(const DGL_Surface *s, int x, int y) {
    return ((size_t)y * (size_t)s->width) + (size_t)x;
}

static int dgl_min_i(int a, int b) { return a < b ? a : b; }
static int dgl_max_i(int a, int b) { return a > b ? a : b; }

static int dgl_out_code(const DGL_Surface *s, double x, double y) {
    int code = 0;
    if (x < 0.0) {
        code |= DGL_OUT_LEFT;
    } else if (x > (double)(s->width - 1)) {
        code |= DGL_OUT_RIGHT;
    }
    if (y < 0.0) {
        code |= DGL_OUT_TOP;
    } else if (y > (double)(s->height - 1)) {
        code |= DGL_OUT_BOTTOM;
    }
    return code;
}

static int dgl_clip_line(const DGL_Surface *s, int *x0, int *y0, int *x1, int *y1) {
    double ax = (double)*x0;
    double ay = (double)*y0;
    double bx = (double)*x1;
    double by = (double)*y1;
    int ac;
    int bc;

    if (!dgl_surface_valid(s)) {
        return 0;
    }

    ac = dgl_out_code(s, ax, ay);
    bc = dgl_out_code(s, bx, by);

    for (;;) {
        double x;
        double y;
        int out;

        if ((ac | bc) == 0) {
            if (ax < 0.0) { ax = 0.0; }
            if (ay < 0.0) { ay = 0.0; }
            if (bx < 0.0) { bx = 0.0; }
            if (by < 0.0) { by = 0.0; }
            if (ax > (double)(s->width - 1)) { ax = (double)(s->width - 1); }
            if (bx > (double)(s->width - 1)) { bx = (double)(s->width - 1); }
            if (ay > (double)(s->height - 1)) { ay = (double)(s->height - 1); }
            if (by > (double)(s->height - 1)) { by = (double)(s->height - 1); }
            *x0 = (int)lrint(ax);
            *y0 = (int)lrint(ay);
            *x1 = (int)lrint(bx);
            *y1 = (int)lrint(by);
            return 1;
        }
        if ((ac & bc) != 0) {
            return 0;
        }

        out = ac != 0 ? ac : bc;
        x = 0.0;
        y = 0.0;

        if ((out & DGL_OUT_TOP) != 0) {
            if (fabs(by - ay) < DBL_EPSILON) { return 0; }
            x = ax + (bx - ax) * (0.0 - ay) / (by - ay);
            y = 0.0;
        } else if ((out & DGL_OUT_BOTTOM) != 0) {
            if (fabs(by - ay) < DBL_EPSILON) { return 0; }
            x = ax + (bx - ax) * ((double)(s->height - 1) - ay) / (by - ay);
            y = (double)(s->height - 1);
        } else if ((out & DGL_OUT_RIGHT) != 0) {
            if (fabs(bx - ax) < DBL_EPSILON) { return 0; }
            y = ay + (by - ay) * ((double)(s->width - 1) - ax) / (bx - ax);
            x = (double)(s->width - 1);
        } else if ((out & DGL_OUT_LEFT) != 0) {
            if (fabs(bx - ax) < DBL_EPSILON) { return 0; }
            y = ay + (by - ay) * (0.0 - ax) / (bx - ax);
            x = 0.0;
        }

        if (out == ac) {
            ax = x;
            ay = y;
            ac = dgl_out_code(s, ax, ay);
        } else {
            bx = x;
            by = y;
            bc = dgl_out_code(s, bx, by);
        }
    }
}

static double dgl_edge(double ax, double ay, double bx, double by, double px, double py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

static uint32_t dgl_mix_color(uint32_t ca, uint32_t cb, uint32_t cc, double wa, double wb, double wc) {
    unsigned int a = (unsigned int)((double)((ca >> 24) & 255u) * wa + (double)((cb >> 24) & 255u) * wb + (double)((cc >> 24) & 255u) * wc + 0.5);
    unsigned int r = (unsigned int)((double)((ca >> 16) & 255u) * wa + (double)((cb >> 16) & 255u) * wb + (double)((cc >> 16) & 255u) * wc + 0.5);
    unsigned int g = (unsigned int)((double)((ca >> 8) & 255u) * wa + (double)((cb >> 8) & 255u) * wb + (double)((cc >> 8) & 255u) * wc + 0.5);
    unsigned int b = (unsigned int)((double)(ca & 255u) * wa + (double)(cb & 255u) * wb + (double)(cc & 255u) * wc + 0.5);
    if (a > 255u) { a = 255u; }
    if (r > 255u) { r = 255u; }
    if (g > 255u) { g = 255u; }
    if (b > 255u) { b = 255u; }
    return DGL_ARGB(a, r, g, b);
}

const char *dgl_version(void) {
    return DGL_VERSION;
}

const char *dgl_result_name(int result) {
    switch (result) {
    case DGL_OK: return "DGL_OK";
    case DGL_ERR_ARG: return "DGL_ERR_ARG";
    case DGL_ERR_ALLOC: return "DGL_ERR_ALLOC";
    case DGL_ERR_RANGE: return "DGL_ERR_RANGE";
    case DGL_ERR_IO: return "DGL_ERR_IO";
    case DGL_ERR_PARSE: return "DGL_ERR_PARSE";
    default: return "DGL_ERR_UNKNOWN";
    }
}

int dgl_surface_init(DGL_Surface *s, int width, int height) {
    size_t count;
    uint32_t *pixels;
    float *depth;

    if (s == NULL || width <= 0 || height <= 0 || width > DGL_MAX_DIM || height > DGL_MAX_DIM) {
        return DGL_ERR_ARG;
    }

    count = (size_t)width * (size_t)height;
    if (count == 0u || count > (size_t)DGL_MAX_PIXELS) {
        return DGL_ERR_RANGE;
    }
    if (count > ((size_t)-1 / sizeof(uint32_t)) || count > ((size_t)-1 / sizeof(float))) {
        return DGL_ERR_RANGE;
    }

    pixels = (uint32_t *)calloc(count, sizeof(uint32_t));
    depth = (float *)malloc(count * sizeof(float));
    if (pixels == NULL || depth == NULL) {
        free(pixels);
        free(depth);
        return DGL_ERR_ALLOC;
    }

    s->width = width;
    s->height = height;
    s->pixels = pixels;
    s->depth = depth;
    dgl_clear_depth(s, DGL_FAR);
    return DGL_OK;
}

void dgl_surface_free(DGL_Surface *s) {
    if (s == NULL) {
        return;
    }
    free(s->pixels);
    free(s->depth);
    s->pixels = NULL;
    s->depth = NULL;
    s->width = 0;
    s->height = 0;
}

int dgl_surface_valid(const DGL_Surface *s) {
    return s != NULL && s->width > 0 && s->height > 0 && s->pixels != NULL && s->depth != NULL;
}

size_t dgl_surface_pixel_count(const DGL_Surface *s) {
    if (!dgl_surface_valid(s)) {
        return 0u;
    }
    return (size_t)s->width * (size_t)s->height;
}

void dgl_clear(DGL_Surface *s, uint32_t color) {
    size_t i;
    size_t count;
    if (!dgl_surface_valid(s)) {
        return;
    }
    count = dgl_surface_pixel_count(s);
    for (i = 0u; i < count; i++) {
        s->pixels[i] = color;
    }
}

void dgl_clear_depth(DGL_Surface *s, float value) {
    size_t i;
    size_t count;
    if (s == NULL || s->depth == NULL || s->width <= 0 || s->height <= 0) {
        return;
    }
    count = (size_t)s->width * (size_t)s->height;
    for (i = 0u; i < count; i++) {
        s->depth[i] = value;
    }
}

void dgl_pixel(DGL_Surface *s, int x, int y, uint32_t color) {
    if (!dgl_inside(s, x, y)) {
        return;
    }
    s->pixels[dgl_index(s, x, y)] = color;
}

uint32_t dgl_get_pixel(const DGL_Surface *s, int x, int y) {
    if (!dgl_inside(s, x, y)) {
        return 0u;
    }
    return s->pixels[dgl_index(s, x, y)];
}

float dgl_get_depth(const DGL_Surface *s, int x, int y) {
    if (!dgl_inside(s, x, y)) {
        return DGL_FAR;
    }
    return s->depth[dgl_index(s, x, y)];
}

void dgl_line(DGL_Surface *s, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx;
    int sx;
    int dy;
    int sy;
    int err;

    if (!dgl_clip_line(s, &x0, &y0, &x1, &y1)) {
        return;
    }

    dx = abs(x1 - x0);
    sx = x0 < x1 ? 1 : -1;
    dy = -abs(y1 - y0);
    sy = y0 < y1 ? 1 : -1;
    err = dx + dy;

    for (;;) {
        int e2;
        dgl_pixel(s, x0, y0, color);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        e2 = err * 2;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void dgl_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color) {
    long long x1;
    long long y1;
    if (w <= 0 || h <= 0) {
        return;
    }
    x1 = (long long)x + (long long)w - 1ll;
    y1 = (long long)y + (long long)h - 1ll;
    if (x1 < (long long)INT_MIN || x1 > (long long)INT_MAX || y1 < (long long)INT_MIN || y1 > (long long)INT_MAX) {
        return;
    }
    dgl_line(s, x, y, (int)x1, y, color);
    dgl_line(s, x, y, x, (int)y1, color);
    dgl_line(s, (int)x1, y, (int)x1, (int)y1, color);
    dgl_line(s, x, (int)y1, (int)x1, (int)y1, color);
}

void dgl_fill_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color) {
    long long rx0;
    long long ry0;
    long long rx1;
    long long ry1;
    int yy;

    if (!dgl_surface_valid(s) || w <= 0 || h <= 0) {
        return;
    }

    rx0 = (long long)x;
    ry0 = (long long)y;
    rx1 = (long long)x + (long long)w;
    ry1 = (long long)y + (long long)h;

    if (rx1 <= 0ll || ry1 <= 0ll || rx0 >= (long long)s->width || ry0 >= (long long)s->height) {
        return;
    }

    if (rx0 < 0ll) { rx0 = 0ll; }
    if (ry0 < 0ll) { ry0 = 0ll; }
    if (rx1 > (long long)s->width) { rx1 = (long long)s->width; }
    if (ry1 > (long long)s->height) { ry1 = (long long)s->height; }

    for (yy = (int)ry0; yy < (int)ry1; yy++) {
        int xx;
        size_t row = (size_t)yy * (size_t)s->width;
        for (xx = (int)rx0; xx < (int)rx1; xx++) {
            s->pixels[row + (size_t)xx] = color;
        }
    }
}

void dgl_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color) {
    int x;
    int y;
    int err;
    if (!dgl_surface_valid(s) || r < 0 || r > DGL_MAX_DIM * 2) {
        return;
    }
    x = r;
    y = 0;
    err = 1 - x;
    while (x >= y) {
        dgl_pixel(s, cx + x, cy + y, color);
        dgl_pixel(s, cx + y, cy + x, color);
        dgl_pixel(s, cx - y, cy + x, color);
        dgl_pixel(s, cx - x, cy + y, color);
        dgl_pixel(s, cx - x, cy - y, color);
        dgl_pixel(s, cx - y, cy - x, color);
        dgl_pixel(s, cx + y, cy - x, color);
        dgl_pixel(s, cx + x, cy - y, color);
        y++;
        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x + 1);
        }
    }
}

void dgl_fill_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color) {
    int y;
    if (!dgl_surface_valid(s) || r < 0 || r > DGL_MAX_DIM * 2) {
        return;
    }
    for (y = -r; y <= r; y++) {
        int rr = (r * r) - (y * y);
        int span = rr <= 0 ? 0 : (int)sqrt((double)rr);
        dgl_line(s, cx - span, cy + y, cx + span, cy + y, color);
    }
}

void dgl_wire_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t color) {
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(b.x) || !isfinite(b.y) || !isfinite(c.x) || !isfinite(c.y)) {
        return;
    }
    dgl_line(s, (int)lrintf(a.x), (int)lrintf(a.y), (int)lrintf(b.x), (int)lrintf(b.y), color);
    dgl_line(s, (int)lrintf(b.x), (int)lrintf(b.y), (int)lrintf(c.x), (int)lrintf(c.y), color);
    dgl_line(s, (int)lrintf(c.x), (int)lrintf(c.y), (int)lrintf(a.x), (int)lrintf(a.y), color);
}

void dgl_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c) {
    double ax;
    double ay;
    double bx;
    double by;
    double cx;
    double cy;
    double area;
    int minx;
    int maxx;
    int miny;
    int maxy;
    int y;

    if (!dgl_surface_valid(s)) {
        return;
    }
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z) || !isfinite(b.x) || !isfinite(b.y) || !isfinite(b.z) || !isfinite(c.x) || !isfinite(c.y) || !isfinite(c.z)) {
        return;
    }

    ax = (double)a.x;
    ay = (double)a.y;
    bx = (double)b.x;
    by = (double)b.y;
    cx = (double)c.x;
    cy = (double)c.y;

    area = dgl_edge(ax, ay, bx, by, cx, cy);
    if (fabs(area) < 1.0e-9) {
        return;
    }

    minx = dgl_max_i(0, (int)floor(fmin(ax, fmin(bx, cx))));
    maxx = dgl_min_i(s->width - 1, (int)ceil(fmax(ax, fmax(bx, cx))));
    miny = dgl_max_i(0, (int)floor(fmin(ay, fmin(by, cy))));
    maxy = dgl_min_i(s->height - 1, (int)ceil(fmax(ay, fmax(by, cy))));

    if (maxx < minx || maxy < miny) {
        return;
    }

    for (y = miny; y <= maxy; y++) {
        int x;
        for (x = minx; x <= maxx; x++) {
            double px = (double)x + 0.5;
            double py = (double)y + 0.5;
            double wa = dgl_edge(bx, by, cx, cy, px, py) / area;
            double wb = dgl_edge(cx, cy, ax, ay, px, py) / area;
            double wc = 1.0 - wa - wb;
            if (wa >= -1.0e-9 && wb >= -1.0e-9 && wc >= -1.0e-9) {
                float z = (float)(wa * (double)a.z + wb * (double)b.z + wc * (double)c.z);
                size_t idx = dgl_index(s, x, y);
                if (z < s->depth[idx]) {
                    s->depth[idx] = z;
                    s->pixels[idx] = dgl_mix_color(a.color, b.color, c.color, wa, wb, wc);
                }
            }
        }
    }
}

uint64_t dgl_hash(const DGL_Surface *s) {
    uint64_t h = 1469598103934665603ull;
    size_t i;
    size_t count;
    if (!dgl_surface_valid(s)) {
        return 0u;
    }
    h ^= (uint64_t)(uint32_t)s->width;
    h *= 1099511628211ull;
    h ^= (uint64_t)(uint32_t)s->height;
    h *= 1099511628211ull;
    count = dgl_surface_pixel_count(s);
    for (i = 0u; i < count; i++) {
        uint32_t p = s->pixels[i];
        h ^= (uint64_t)(p & 255u);
        h *= 1099511628211ull;
        h ^= (uint64_t)((p >> 8) & 255u);
        h *= 1099511628211ull;
        h ^= (uint64_t)((p >> 16) & 255u);
        h *= 1099511628211ull;
        h ^= (uint64_t)((p >> 24) & 255u);
        h *= 1099511628211ull;
    }
    return h;
}

int dgl_save_ppm(const DGL_Surface *s, const char *path) {
    FILE *f;
    int y;
    if (!dgl_surface_valid(s) || path == NULL || path[0] == '\0') {
        return DGL_ERR_ARG;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return DGL_ERR_IO;
    }
    if (fprintf(f, "P6\n%d %d\n255\n", s->width, s->height) < 0) {
        fclose(f);
        return DGL_ERR_IO;
    }
    for (y = 0; y < s->height; y++) {
        int x;
        for (x = 0; x < s->width; x++) {
            uint32_t p = s->pixels[dgl_index(s, x, y)];
            unsigned char rgb[3];
            rgb[0] = (unsigned char)((p >> 16) & 255u);
            rgb[1] = (unsigned char)((p >> 8) & 255u);
            rgb[2] = (unsigned char)(p & 255u);
            if (fwrite(rgb, 1u, 3u, f) != 3u) {
                fclose(f);
                return DGL_ERR_IO;
            }
        }
    }
    if (fclose(f) != 0) {
        return DGL_ERR_IO;
    }
    return DGL_OK;
}

int dgl_parse_color(const char *text, uint32_t *out_color) {
    const char *p;
    uint32_t value = 0u;
    int digits = 0;

    if (text == NULL || out_color == NULL) {
        return DGL_ERR_ARG;
    }
    p = text;
    if (*p == '#') {
        p++;
    } else if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    while (*p != '\0' && *p != '\n' && *p != '\r') {
        int v;
        if (!isxdigit((unsigned char)*p)) {
            return DGL_ERR_PARSE;
        }
        if (*p >= '0' && *p <= '9') {
            v = *p - '0';
        } else if (*p >= 'a' && *p <= 'f') {
            v = *p - 'a' + 10;
        } else if (*p >= 'A' && *p <= 'F') {
            v = *p - 'A' + 10;
        } else {
            return DGL_ERR_PARSE;
        }
        if (digits >= 8) {
            return DGL_ERR_PARSE;
        }
        value = (value << 4) | (uint32_t)v;
        digits++;
        p++;
    }
    if (digits != 6 && digits != 8) {
        return DGL_ERR_PARSE;
    }
    if (digits == 6) {
        value |= 0xff000000u;
    }
    *out_color = value;
    return DGL_OK;
}
