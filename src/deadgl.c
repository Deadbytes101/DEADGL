#include "deadgl.h"
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIRE_EPS 0.025f
#define TRI_EPS 1.0e-9
#define DEPTH_EPS 1.0e-6f

static int ok(const DGL_Surface *s) {
    return s != NULL && s->width > 0 && s->height > 0 && s->pixels != NULL && s->depth != NULL;
}

static int inb(const DGL_Surface *s, int x, int y) {
    return ok(s) && x >= 0 && y >= 0 && x < s->width && y < s->height;
}

static size_t at(const DGL_Surface *s, int x, int y) {
    return (size_t)y * (size_t)s->width + (size_t)x;
}

static int min_int(int a, int b) {
    return a < b ? a : b;
}

static int max_int(int a, int b) {
    return a > b ? a : b;
}

static long long ll_abs_delta(int a, int b) {
    long long d = (long long)a - (long long)b;
    return d < 0 ? -d : d;
}

static double edge_value(double ax, double ay, double bx, double by, double px, double py) {
    return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

static unsigned int byte_from_double(double v) {
    if (!isfinite(v) || v <= 0.0) { return 0u; }
    if (v >= 255.0) { return 255u; }
    return (unsigned int)(v + 0.5);
}

static uint32_t mixc(uint32_t a, uint32_t b, uint32_t c, double wa, double wb, double wc) {
    unsigned int ar = byte_from_double((double)((a >> 24) & 255u) * wa + (double)((b >> 24) & 255u) * wb + (double)((c >> 24) & 255u) * wc);
    unsigned int rr = byte_from_double((double)((a >> 16) & 255u) * wa + (double)((b >> 16) & 255u) * wb + (double)((c >> 16) & 255u) * wc);
    unsigned int gg = byte_from_double((double)((a >> 8) & 255u) * wa + (double)((b >> 8) & 255u) * wb + (double)((c >> 8) & 255u) * wc);
    unsigned int bb = byte_from_double((double)(a & 255u) * wa + (double)(b & 255u) * wb + (double)(c & 255u) * wc);
    return DGL_ARGB(ar, rr, gg, bb);
}

static unsigned char glyph_row(char ch, int row) {
    unsigned int r;
    unsigned int c;
    unsigned int core;
    if (row < 0 || row >= 7) { return 0u; }
    if (ch == ' ') { return 0u; }
    if (row == 0 || row == 6) { return 31u; }
    r = (unsigned int)row;
    c = (unsigned int)(unsigned char)ch;
    core = ((c >> (r - 1u)) ^ (c * 13u) ^ (r * 29u)) & 14u;
    return (unsigned char)(17u | core);
}

static int clip_test(double p, double q, double *t0, double *t1) {
    double r;
    if (p == 0.0) { return q >= 0.0; }
    r = q / p;
    if (p < 0.0) {
        if (r > *t1) { return 0; }
        if (r > *t0) { *t0 = r; }
    } else {
        if (r < *t0) { return 0; }
        if (r < *t1) { *t1 = r; }
    }
    return 1;
}

static int clip_line_d(const DGL_Surface *s, double x0, double y0, double x1, double y1, double *t0, double *t1) {
    double dx;
    double dy;
    double xmax;
    double ymax;
    if (!ok(s) || t0 == NULL || t1 == NULL) { return 0; }
    if (!isfinite(x0) || !isfinite(y0) || !isfinite(x1) || !isfinite(y1)) { return 0; }
    dx = x1 - x0;
    dy = y1 - y0;
    xmax = (double)(s->width - 1);
    ymax = (double)(s->height - 1);
    *t0 = 0.0;
    *t1 = 1.0;
    return clip_test(-dx, x0, t0, t1) &&
           clip_test(dx, xmax - x0, t0, t1) &&
           clip_test(-dy, y0, t0, t1) &&
           clip_test(dy, ymax - y0, t0, t1);
}

static int round_to_int(double v) {
    if (v <= (double)INT_MIN) { return INT_MIN; }
    if (v >= (double)INT_MAX) { return INT_MAX; }
    return (int)lrint(v);
}

const char *dgl_version(void) {
    return DGL_VERSION;
}

const char *dgl_result_name(int r) {
    switch (r) {
        case DGL_OK: return "DGL_OK";
        case DGL_ERR_ARG: return "DGL_ERR_ARG";
        case DGL_ERR_ALLOC: return "DGL_ERR_ALLOC";
        case DGL_ERR_RANGE: return "DGL_ERR_RANGE";
        case DGL_ERR_IO: return "DGL_ERR_IO";
        case DGL_ERR_PARSE: return "DGL_ERR_PARSE";
        default: return "DGL_ERR_UNKNOWN";
    }
}

int dgl_surface_init(DGL_Surface *s, int w, int h) {
    size_t n;
    if (s == NULL || w <= 0 || h <= 0 || w > DGL_MAX_DIM || h > DGL_MAX_DIM) { return DGL_ERR_ARG; }
    n = (size_t)w * (size_t)h;
    if (n == 0u || n > (size_t)DGL_MAX_PIXELS || n > ((size_t)-1 / sizeof(uint32_t)) || n > ((size_t)-1 / sizeof(float))) { return DGL_ERR_RANGE; }
    s->pixels = (uint32_t *)calloc(n, sizeof(uint32_t));
    s->depth = (float *)malloc(n * sizeof(float));
    if (s->pixels == NULL || s->depth == NULL) {
        free(s->pixels);
        free(s->depth);
        s->pixels = NULL;
        s->depth = NULL;
        return DGL_ERR_ALLOC;
    }
    s->width = w;
    s->height = h;
    dgl_clear_depth(s, DGL_FAR);
    return DGL_OK;
}

void dgl_surface_free(DGL_Surface *s) {
    if (s != NULL) {
        free(s->pixels);
        free(s->depth);
        s->pixels = NULL;
        s->depth = NULL;
        s->width = 0;
        s->height = 0;
    }
}

int dgl_surface_valid(const DGL_Surface *s) {
    return ok(s);
}

size_t dgl_surface_pixel_count(const DGL_Surface *s) {
    return ok(s) ? (size_t)s->width * (size_t)s->height : 0u;
}

void dgl_clear(DGL_Surface *s, uint32_t c) {
    size_t i;
    size_t n = dgl_surface_pixel_count(s);
    for (i = 0u; i < n; i++) { s->pixels[i] = c; }
}

void dgl_clear_depth(DGL_Surface *s, float z) {
    size_t i;
    size_t n;
    if (s == NULL || s->depth == NULL || s->width <= 0 || s->height <= 0) { return; }
    n = (size_t)s->width * (size_t)s->height;
    for (i = 0u; i < n; i++) { s->depth[i] = z; }
}

void dgl_pixel(DGL_Surface *s, int x, int y, uint32_t c) {
    if (inb(s, x, y)) { s->pixels[at(s, x, y)] = c; }
}

uint32_t dgl_get_pixel(const DGL_Surface *s, int x, int y) {
    return inb(s, x, y) ? s->pixels[at(s, x, y)] : 0u;
}

float dgl_get_depth(const DGL_Surface *s, int x, int y) {
    return inb(s, x, y) ? s->depth[at(s, x, y)] : DGL_FAR;
}

void dgl_line(DGL_Surface *s, int x0, int y0, int x1, int y1, uint32_t c) {
    double t0;
    double t1;
    int sx;
    int sy;
    int ex;
    int ey;
    int dx;
    int dy;
    int step_x;
    int step_y;
    int err;
    if (!ok(s)) { return; }
    if (!clip_line_d(s, (double)x0, (double)y0, (double)x1, (double)y1, &t0, &t1)) { return; }
    sx = round_to_int((double)x0 + ((double)x1 - (double)x0) * t0);
    sy = round_to_int((double)y0 + ((double)y1 - (double)y0) * t0);
    ex = round_to_int((double)x0 + ((double)x1 - (double)x0) * t1);
    ey = round_to_int((double)y0 + ((double)y1 - (double)y0) * t1);
    dx = (int)ll_abs_delta(ex, sx);
    dy = -(int)ll_abs_delta(ey, sy);
    step_x = sx < ex ? 1 : -1;
    step_y = sy < ey ? 1 : -1;
    err = dx + dy;
    for (;;) {
        int e2;
        dgl_pixel(s, sx, sy, c);
        if (sx == ex && sy == ey) { break; }
        e2 = err * 2;
        if (e2 >= dy) { err += dy; sx += step_x; }
        if (e2 <= dx) { err += dx; sy += step_y; }
    }
}

void dgl_depth_line(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, uint32_t c) {
    double t0;
    double t1;
    int x0;
    int y0;
    int x1;
    int y1;
    long long nll;
    int n;
    int i;
    if (!ok(s) || !isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z) || !isfinite(b.x) || !isfinite(b.y) || !isfinite(b.z)) { return; }
    if (!clip_line_d(s, (double)a.x, (double)a.y, (double)b.x, (double)b.y, &t0, &t1)) { return; }
    x0 = round_to_int((double)a.x + ((double)b.x - (double)a.x) * t0);
    y0 = round_to_int((double)a.y + ((double)b.y - (double)a.y) * t0);
    x1 = round_to_int((double)a.x + ((double)b.x - (double)a.x) * t1);
    y1 = round_to_int((double)a.y + ((double)b.y - (double)a.y) * t1);
    nll = ll_abs_delta(x1, x0);
    if (ll_abs_delta(y1, y0) > nll) { nll = ll_abs_delta(y1, y0); }
    if (nll > (long long)DGL_MAX_DIM * 8LL) { return; }
    n = (int)nll;
    for (i = 0; i <= n; i++) {
        double local = n == 0 ? 0.0 : (double)i / (double)n;
        double t = t0 + (t1 - t0) * local;
        int x = round_to_int((double)a.x + ((double)b.x - (double)a.x) * t);
        int y = round_to_int((double)a.y + ((double)b.y - (double)a.y) * t);
        float z = (float)((double)a.z + ((double)b.z - (double)a.z) * t);
        if (inb(s, x, y)) {
            size_t p = at(s, x, y);
            if (z <= s->depth[p] + WIRE_EPS) {
                s->depth[p] = z;
                s->pixels[p] = c;
            }
        }
    }
}

void dgl_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t c) {
    long long x1;
    long long y1;
    if (w <= 0 || h <= 0) { return; }
    x1 = (long long)x + (long long)w - 1LL;
    y1 = (long long)y + (long long)h - 1LL;
    if (x1 < (long long)INT_MIN || x1 > (long long)INT_MAX || y1 < (long long)INT_MIN || y1 > (long long)INT_MAX) { return; }
    dgl_line(s, x, y, (int)x1, y, c);
    dgl_line(s, x, y, x, (int)y1, c);
    dgl_line(s, (int)x1, y, (int)x1, (int)y1, c);
    dgl_line(s, x, (int)y1, (int)x1, (int)y1, c);
}

void dgl_fill_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t c) {
    long long raw_x0;
    long long raw_y0;
    long long raw_x1;
    long long raw_y1;
    int x0;
    int y0;
    int x1;
    int y1;
    int yy;
    if (!ok(s) || w <= 0 || h <= 0) { return; }
    raw_x0 = (long long)x;
    raw_y0 = (long long)y;
    raw_x1 = (long long)x + (long long)w;
    raw_y1 = (long long)y + (long long)h;
    if (raw_x1 <= 0LL || raw_y1 <= 0LL || raw_x0 >= (long long)s->width || raw_y0 >= (long long)s->height) { return; }
    x0 = (int)(raw_x0 < 0LL ? 0LL : raw_x0);
    y0 = (int)(raw_y0 < 0LL ? 0LL : raw_y0);
    x1 = (int)(raw_x1 > (long long)s->width ? (long long)s->width : raw_x1);
    y1 = (int)(raw_y1 > (long long)s->height ? (long long)s->height : raw_y1);
    for (yy = y0; yy < y1; yy++) {
        int xx;
        size_t row = (size_t)yy * (size_t)s->width;
        for (xx = x0; xx < x1; xx++) { s->pixels[row + (size_t)xx] = c; }
    }
}

void dgl_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t c) {
    int x = r;
    int y = 0;
    int e = 1 - x;
    if (!ok(s) || r < 0 || r > DGL_MAX_DIM * 2) { return; }
    while (x >= y) {
        dgl_pixel(s, cx + x, cy + y, c);
        dgl_pixel(s, cx + y, cy + x, c);
        dgl_pixel(s, cx - y, cy + x, c);
        dgl_pixel(s, cx - x, cy + y, c);
        dgl_pixel(s, cx - x, cy - y, c);
        dgl_pixel(s, cx - y, cy - x, c);
        dgl_pixel(s, cx + y, cy - x, c);
        dgl_pixel(s, cx + x, cy - y, c);
        y++;
        if (e < 0) { e += 2 * y + 1; }
        else { x--; e += 2 * (y - x + 1); }
    }
}

void dgl_fill_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t c) {
    int y;
    if (!ok(s) || r < 0 || r > DGL_MAX_DIM * 2) { return; }
    for (y = -r; y <= r; y++) {
        int rr = r * r - y * y;
        int sp = rr <= 0 ? 0 : (int)sqrt((double)rr);
        dgl_line(s, cx - sp, cy + y, cx + sp, cy + y, c);
    }
}

void dgl_wire_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t col) {
    dgl_line(s, (int)lrintf(a.x), (int)lrintf(a.y), (int)lrintf(b.x), (int)lrintf(b.y), col);
    dgl_line(s, (int)lrintf(b.x), (int)lrintf(b.y), (int)lrintf(c.x), (int)lrintf(c.y), col);
    dgl_line(s, (int)lrintf(c.x), (int)lrintf(c.y), (int)lrintf(a.x), (int)lrintf(a.y), col);
}

void dgl_wire_tri_depth(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t col) {
    dgl_depth_line(s, a, b, col);
    dgl_depth_line(s, b, c, col);
    dgl_depth_line(s, c, a, col);
}

void dgl_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c) {
    double ax;
    double ay;
    double bx;
    double by;
    double cx;
    double cy;
    double area;
    int x;
    int y;
    int minx;
    int maxx;
    int miny;
    int maxy;
    if (!ok(s) || !isfinite(a.x) || !isfinite(a.y) || !isfinite(a.z) || !isfinite(b.x) || !isfinite(b.y) || !isfinite(b.z) || !isfinite(c.x) || !isfinite(c.y) || !isfinite(c.z)) { return; }
    ax = (double)a.x;
    ay = (double)a.y;
    bx = (double)b.x;
    by = (double)b.y;
    cx = (double)c.x;
    cy = (double)c.y;
    area = edge_value(ax, ay, bx, by, cx, cy);
    if (!isfinite(area) || fabs(area) < TRI_EPS) { return; }
    minx = max_int(0, (int)ceil(fmin(ax, fmin(bx, cx)) - 0.5));
    maxx = min_int(s->width - 1, (int)floor(fmax(ax, fmax(bx, cx)) - 0.5));
    miny = max_int(0, (int)ceil(fmin(ay, fmin(by, cy)) - 0.5));
    maxy = min_int(s->height - 1, (int)floor(fmax(ay, fmax(by, cy)) - 0.5));
    if (minx > maxx || miny > maxy) { return; }
    for (y = miny; y <= maxy; y++) {
        for (x = minx; x <= maxx; x++) {
            double px = (double)x + 0.5;
            double py = (double)y + 0.5;
            double e0 = edge_value(bx, by, cx, cy, px, py);
            double e1 = edge_value(cx, cy, ax, ay, px, py);
            double e2 = edge_value(ax, ay, bx, by, px, py);
            double wa;
            double wb;
            double wc;
            float z;
            uint32_t color;
            size_t p;
            if (e0 * area < -TRI_EPS || e1 * area < -TRI_EPS || e2 * area < -TRI_EPS) { continue; }
            wa = e0 / area;
            wb = e1 / area;
            wc = 1.0 - wa - wb;
            if (a.z > DEPTH_EPS && b.z > DEPTH_EPS && c.z > DEPTH_EPS) {
                double ia = 1.0 / (double)a.z;
                double ib = 1.0 / (double)b.z;
                double ic = 1.0 / (double)c.z;
                double denom = wa * ia + wb * ib + wc * ic;
                if (!isfinite(denom) || denom <= 0.0) { continue; }
                wa = (wa * ia) / denom;
                wb = (wb * ib) / denom;
                wc = 1.0 - wa - wb;
                z = (float)(1.0 / denom);
            } else {
                z = (float)(wa * (double)a.z + wb * (double)b.z + wc * (double)c.z);
            }
            if (!isfinite(z)) { continue; }
            p = at(s, x, y);
            if (z < s->depth[p]) {
                s->depth[p] = z;
                color = mixc(a.color, b.color, c.color, wa, wb, wc);
                s->pixels[p] = color;
            }
        }
    }
}

void dgl_quad(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, DGL_Vertex d) {
    dgl_tri(s, a, b, c);
    dgl_tri(s, a, c, d);
}

void dgl_wire_quad_depth(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, DGL_Vertex d, uint32_t col) {
    dgl_depth_line(s, a, b, col);
    dgl_depth_line(s, b, c, col);
    dgl_depth_line(s, c, d, col);
    dgl_depth_line(s, d, a, col);
}

void dgl_char(DGL_Surface *s, int x, int y, uint32_t color, char ch) {
    int yy;
    if (!ok(s)) { return; }
    for (yy = 0; yy < 7; yy++) {
        unsigned char row = glyph_row(ch, yy);
        int xx;
        for (xx = 0; xx < 5; xx++) {
            if ((row & (1u << (4 - xx))) != 0u) { dgl_pixel(s, x + xx, y + yy, color); }
        }
    }
}

void dgl_text(DGL_Surface *s, int x, int y, uint32_t color, const char *text) {
    int pen = x;
    if (!ok(s) || text == NULL) { return; }
    while (*text != '\0') {
        if (*text == '\n') { y += 8; pen = x; }
        else { dgl_char(s, pen, y, color, *text); pen += 6; }
        text++;
    }
}

uint64_t dgl_hash(const DGL_Surface *s) {
    uint64_t h = 1469598103934665603ull;
    size_t i;
    size_t n = dgl_surface_pixel_count(s);
    if (!ok(s)) { return 0u; }
    h ^= (uint64_t)(uint32_t)s->width;
    h *= 1099511628211ull;
    h ^= (uint64_t)(uint32_t)s->height;
    h *= 1099511628211ull;
    for (i = 0u; i < n; i++) {
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
    if (!ok(s) || path == NULL || path[0] == '\0') { return DGL_ERR_ARG; }
    f = fopen(path, "wb");
    if (f == NULL) { return DGL_ERR_IO; }
    if (fprintf(f, "P6\n%d %d\n255\n", s->width, s->height) < 0) { fclose(f); return DGL_ERR_IO; }
    for (y = 0; y < s->height; y++) {
        int x;
        for (x = 0; x < s->width; x++) {
            uint32_t p = s->pixels[at(s, x, y)];
            unsigned char rgb[3];
            rgb[0] = (unsigned char)((p >> 16) & 255u);
            rgb[1] = (unsigned char)((p >> 8) & 255u);
            rgb[2] = (unsigned char)(p & 255u);
            if (fwrite(rgb, 1u, 3u, f) != 3u) { fclose(f); return DGL_ERR_IO; }
        }
    }
    return fclose(f) == 0 ? DGL_OK : DGL_ERR_IO;
}

int dgl_parse_color(const char *t, uint32_t *out) {
    const char *p;
    uint32_t v = 0u;
    int n = 0;
    if (t == NULL || out == NULL) { return DGL_ERR_ARG; }
    p = t;
    if (*p == '#') { p++; }
    else if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) { p += 2; }
    while (*p != '\0' && *p != '\n' && *p != '\r') {
        int x;
        if (!isxdigit((unsigned char)*p)) { return DGL_ERR_PARSE; }
        x = isdigit((unsigned char)*p) ? *p - '0' : 10 + (tolower((unsigned char)*p) - 'a');
        v = (v << 4) | (uint32_t)x;
        n++;
        p++;
    }
    if (n == 6) { *out = 0xff000000u | v; return DGL_OK; }
    if (n == 8) { *out = v; return DGL_OK; }
    return DGL_ERR_PARSE;
}
