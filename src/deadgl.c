#include "deadgl.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int inside(const DGL_Surface *s, int x, int y) {
    return s != NULL && x >= 0 && y >= 0 && x < s->width && y < s->height;
}

static int index_of(const DGL_Surface *s, int x, int y) {
    return y * s->width + x;
}

static int min_i(int a, int b) { return a < b ? a : b; }
static int max_i(int a, int b) { return a > b ? a : b; }
static float edge(float ax, float ay, float bx, float by, float px, float py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

int dgl_surface_init(DGL_Surface *s, int width, int height) {
    size_t count;
    if (s == NULL || width <= 0 || height <= 0) {
        return DGL_ERR;
    }
    if (width > 16384 || height > 16384) {
        return DGL_ERR;
    }
    count = (size_t)width * (size_t)height;
    if (count > ((size_t)-1 / sizeof(uint32_t))) {
        return DGL_ERR;
    }
    s->width = width;
    s->height = height;
    s->pixels = (uint32_t *)calloc(count, sizeof(uint32_t));
    s->depth = (float *)malloc(count * sizeof(float));
    if (s->pixels == NULL || s->depth == NULL) {
        dgl_surface_free(s);
        return DGL_ERR;
    }
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

void dgl_clear(DGL_Surface *s, uint32_t color) {
    size_t i;
    size_t count;
    if (s == NULL || s->pixels == NULL) {
        return;
    }
    count = (size_t)s->width * (size_t)s->height;
    for (i = 0; i < count; i++) {
        s->pixels[i] = color;
    }
}

void dgl_clear_depth(DGL_Surface *s, float value) {
    size_t i;
    size_t count;
    if (s == NULL || s->depth == NULL) {
        return;
    }
    count = (size_t)s->width * (size_t)s->height;
    for (i = 0; i < count; i++) {
        s->depth[i] = value;
    }
}

void dgl_pixel(DGL_Surface *s, int x, int y, uint32_t color) {
    if (!inside(s, x, y) || s->pixels == NULL) {
        return;
    }
    s->pixels[index_of(s, x, y)] = color;
}

uint32_t dgl_get_pixel(const DGL_Surface *s, int x, int y) {
    if (!inside(s, x, y) || s->pixels == NULL) {
        return 0u;
    }
    return s->pixels[index_of(s, x, y)];
}

void dgl_line(DGL_Surface *s, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx;
    int sx;
    int dy;
    int sy;
    int err;
    int limit;
    if (s == NULL) {
        return;
    }
    dx = abs(x1 - x0);
    sx = x0 < x1 ? 1 : -1;
    dy = -abs(y1 - y0);
    sy = y0 < y1 ? 1 : -1;
    err = dx + dy;
    limit = (s->width + s->height + abs(x1 - x0) + abs(y1 - y0)) * 2 + 8;
    while (limit-- > 0) {
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
    if (w <= 0 || h <= 0) {
        return;
    }
    dgl_line(s, x, y, x + w - 1, y, color);
    dgl_line(s, x, y, x, y + h - 1, color);
    dgl_line(s, x + w - 1, y, x + w - 1, y + h - 1, color);
    dgl_line(s, x, y + h - 1, x + w - 1, y + h - 1, color);
}

void dgl_fill_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color) {
    int x0;
    int y0;
    int x1;
    int y1;
    int yy;
    if (s == NULL || w <= 0 || h <= 0) {
        return;
    }
    x0 = max_i(x, 0);
    y0 = max_i(y, 0);
    x1 = min_i(x + w, s->width);
    y1 = min_i(y + h, s->height);
    for (yy = y0; yy < y1; yy++) {
        int xx;
        for (xx = x0; xx < x1; xx++) {
            dgl_pixel(s, xx, yy, color);
        }
    }
}

void dgl_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color) {
    int x;
    int y;
    int err;
    if (r < 0) {
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
    if (r < 0) {
        return;
    }
    for (y = -r; y <= r; y++) {
        int span = (int)sqrt((double)(r * r - y * y));
        dgl_line(s, cx - span, cy + y, cx + span, cy + y, color);
    }
}

void dgl_wire_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t color) {
    dgl_line(s, (int)a.x, (int)a.y, (int)b.x, (int)b.y, color);
    dgl_line(s, (int)b.x, (int)b.y, (int)c.x, (int)c.y, color);
    dgl_line(s, (int)c.x, (int)c.y, (int)a.x, (int)a.y, color);
}

void dgl_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c) {
    int minx;
    int maxx;
    int miny;
    int maxy;
    float area;
    int y;
    if (s == NULL || s->pixels == NULL || s->depth == NULL) {
        return;
    }
    if (!isfinite(a.x) || !isfinite(a.y) || !isfinite(b.x) || !isfinite(b.y) || !isfinite(c.x) || !isfinite(c.y)) {
        return;
    }
    area = edge(a.x, a.y, b.x, b.y, c.x, c.y);
    if (fabsf(area) < FLT_EPSILON) {
        return;
    }
    minx = max_i(0, (int)floorf(fminf(a.x, fminf(b.x, c.x))));
    maxx = min_i(s->width - 1, (int)ceilf(fmaxf(a.x, fmaxf(b.x, c.x))));
    miny = max_i(0, (int)floorf(fminf(a.y, fminf(b.y, c.y))));
    maxy = min_i(s->height - 1, (int)ceilf(fmaxf(a.y, fmaxf(b.y, c.y))));
    for (y = miny; y <= maxy; y++) {
        int x;
        for (x = minx; x <= maxx; x++) {
            float px = (float)x + 0.5f;
            float py = (float)y + 0.5f;
            float w0 = edge(b.x, b.y, c.x, c.y, px, py) / area;
            float w1 = edge(c.x, c.y, a.x, a.y, px, py) / area;
            float w2 = 1.0f - w0 - w1;
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                int idx = index_of(s, x, y);
                float z = w0 * a.z + w1 * b.z + w2 * c.z;
                if (z < s->depth[idx]) {
                    s->depth[idx] = z;
                    s->pixels[idx] = a.color;
                }
            }
        }
    }
}

uint64_t dgl_hash(const DGL_Surface *s) {
    uint64_t h = 1469598103934665603ull;
    size_t i;
    size_t count;
    if (s == NULL || s->pixels == NULL) {
        return 0u;
    }
    h ^= (uint64_t)(uint32_t)s->width;
    h *= 1099511628211ull;
    h ^= (uint64_t)(uint32_t)s->height;
    h *= 1099511628211ull;
    count = (size_t)s->width * (size_t)s->height;
    for (i = 0; i < count; i++) {
        h ^= (uint64_t)s->pixels[i];
        h *= 1099511628211ull;
    }
    return h;
}

int dgl_save_ppm(const DGL_Surface *s, const char *path) {
    FILE *f;
    int y;
    if (s == NULL || s->pixels == NULL || path == NULL) {
        return DGL_ERR;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return DGL_ERR;
    }
    if (fprintf(f, "P6\n%d %d\n255\n", s->width, s->height) < 0) {
        fclose(f);
        return DGL_ERR;
    }
    for (y = 0; y < s->height; y++) {
        int x;
        for (x = 0; x < s->width; x++) {
            uint32_t p = s->pixels[index_of(s, x, y)];
            unsigned char rgb[3];
            rgb[0] = (unsigned char)((p >> 16) & 255u);
            rgb[1] = (unsigned char)((p >> 8) & 255u);
            rgb[2] = (unsigned char)(p & 255u);
            if (fwrite(rgb, 1, 3, f) != 3) {
                fclose(f);
                return DGL_ERR;
            }
        }
    }
    fclose(f);
    return DGL_OK;
}
