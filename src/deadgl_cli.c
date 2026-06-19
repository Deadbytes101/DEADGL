#include "deadgl.h"
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_MAX_DGL 1024
#define TOK_MAX 48
#define MAX_VERTS 256
#define CLIP_MAX 8
#define PI_D 3.14159265358979323846

typedef struct V3 { float x; float y; float z; } V3;
typedef struct Basis { V3 f; V3 r; V3 u; int ok; } Basis;
typedef struct Cam { V3 eye; V3 target; V3 up; float fov; float nearz; } Cam;
typedef struct St { DGL_Surface s; const char *path; int line; Cam cam; int cull; int shade; V3 light; V3 vert[MAX_VERTS]; unsigned char have[MAX_VERTS]; } St;

static V3 v3(float x, float y, float z) { V3 v; v.x = x; v.y = y; v.z = z; return v; }
static V3 sub3(V3 a, V3 b) { return v3(a.x - b.x, a.y - b.y, a.z - b.z); }
static V3 cross3(V3 a, V3 b) { return v3((float)((double)a.y * (double)b.z - (double)a.z * (double)b.y), (float)((double)a.z * (double)b.x - (double)a.x * (double)b.z), (float)((double)a.x * (double)b.y - (double)a.y * (double)b.x)); }
static double dot3(V3 a, V3 b) { return (double)a.x * (double)b.x + (double)a.y * (double)b.y + (double)a.z * (double)b.z; }
static int norm3(V3 *v) { double n; if (v == NULL) { return 0; } n = sqrt(dot3(*v, *v)); if (!isfinite(n) || n < 1.0e-9) { return 0; } v->x = (float)((double)v->x / n); v->y = (float)((double)v->y / n); v->z = (float)((double)v->z / n); return 1; }
static V3 lerp3(V3 a, V3 b, double t) { return v3((float)((double)a.x + ((double)b.x - (double)a.x) * t), (float)((double)a.y + ((double)b.y - (double)a.y) * t), (float)((double)a.z + ((double)b.z - (double)a.z) * t)); }
static Cam default_cam(void) { Cam c; c.eye = v3(3.0f, 2.0f, 5.0f); c.target = v3(0.0f, 0.0f, 0.0f); c.up = v3(0.0f, 1.0f, 0.0f); c.fov = 70.0f; c.nearz = 0.05f; return c; }

static Basis make_basis(const Cam *cam) {
    Basis b;
    b.f = sub3(cam->target, cam->eye);
    b.r = v3(1.0f, 0.0f, 0.0f);
    b.u = cam->up;
    b.ok = 0;
    if (!norm3(&b.f)) { return b; }
    if (!norm3(&b.u)) { b.u = v3(0.0f, 1.0f, 0.0f); }
    b.r = cross3(b.f, b.u);
    if (!norm3(&b.r)) { b.r = v3(1.0f, 0.0f, 0.0f); }
    b.u = cross3(b.r, b.f);
    if (!norm3(&b.u)) { return b; }
    b.ok = 1;
    return b;
}

static double depth_of(const Cam *cam, const Basis *b, V3 p) { return dot3(sub3(p, cam->eye), b->f); }

static int clip_near(const Cam *cam, const V3 *in, int n, V3 *out) {
    Basis basis = make_basis(cam);
    int i;
    int m = 0;
    if (!basis.ok || n < 3) { return 0; }
    for (i = 0; i < n; i++) {
        int j = (i + 1) % n;
        V3 a = in[i];
        V3 b = in[j];
        double za = depth_of(cam, &basis, a);
        double zb = depth_of(cam, &basis, b);
        int ina = isfinite(za) && za >= (double)cam->nearz;
        int inb = isfinite(zb) && zb >= (double)cam->nearz;
        if (ina && m < CLIP_MAX) { out[m++] = a; }
        if (ina != inb && fabs(zb - za) > 1.0e-12 && m < CLIP_MAX) {
            double t = ((double)cam->nearz - za) / (zb - za);
            if (t < 0.0) { t = 0.0; }
            if (t > 1.0) { t = 1.0; }
            out[m++] = lerp3(a, b, t);
        }
    }
    return m;
}

static int backface(const Cam *cam, V3 a, V3 b, V3 c) { V3 n = cross3(sub3(b, a), sub3(c, a)); V3 to_eye = sub3(cam->eye, a); return dot3(n, to_eye) <= 0.0; }

static uint32_t shade_col(St *st, V3 a, V3 b, V3 c, uint32_t col) {
    V3 n;
    V3 l;
    double d;
    double k;
    unsigned int aa;
    unsigned int rr;
    unsigned int gg;
    unsigned int bb;
    if (!st->shade) { return col; }
    n = cross3(sub3(b, a), sub3(c, a));
    if (!norm3(&n)) { return col; }
    l = st->light;
    if (!norm3(&l)) { return col; }
    d = fabs(dot3(n, l));
    if (d > 1.0) { d = 1.0; }
    k = 0.25 + 0.75 * d;
    aa = (unsigned int)((col >> 24) & 255u);
    rr = (unsigned int)((double)((col >> 16) & 255u) * k + 0.5);
    gg = (unsigned int)((double)((col >> 8) & 255u) * k + 0.5);
    bb = (unsigned int)((double)(col & 255u) * k + 0.5);
    if (rr > 255u) { rr = 255u; }
    if (gg > 255u) { gg = 255u; }
    if (bb > 255u) { bb = 255u; }
    return DGL_ARGB(aa, rr, gg, bb);
}

static int project(const DGL_Surface *s, const Cam *cam, V3 p, uint32_t color, DGL_Vertex *out) {
    Basis basis;
    V3 rel;
    double cx;
    double cy;
    double cz;
    double scale;
    double sx;
    double sy;
    double fov;
    if (!dgl_surface_valid(s) || cam == NULL || out == NULL) { return 0; }
    basis = make_basis(cam);
    if (!basis.ok) { return 0; }
    rel = sub3(p, cam->eye);
    cx = dot3(rel, basis.r);
    cy = dot3(rel, basis.u);
    cz = dot3(rel, basis.f);
    if (!isfinite(cz) || cz < (double)cam->nearz) { return 0; }
    fov = cam->fov > 1.0f ? (double)cam->fov : 70.0;
    if (fov > 170.0) { fov = 170.0; }
    scale = ((double)s->height * 0.5) / tan((fov * PI_D / 180.0) * 0.5);
    sx = ((double)s->width - 1.0) * 0.5 + (cx * scale / cz);
    sy = ((double)s->height - 1.0) * 0.5 - (cy * scale / cz);
    if (!isfinite(sx) || !isfinite(sy)) { return 0; }
    out->x = (float)sx;
    out->y = (float)sy;
    out->z = (float)cz;
    out->color = color;
    return 1;
}

static void draw_poly(St *st, V3 *poly, int n, V3 a, V3 b, V3 c, uint32_t col, int wire) {
    DGL_Vertex pv[CLIP_MAX];
    uint32_t draw = wire ? col : shade_col(st, a, b, c, col);
    int i;
    if (n < 3 || n > CLIP_MAX) { return; }
    for (i = 0; i < n; i++) {
        if (!project(&st->s, &st->cam, poly[i], draw, &pv[i])) { return; }
    }
    if (wire) {
        for (i = 0; i < n; i++) { dgl_depth_line(&st->s, pv[i], pv[(i + 1) % n], col); }
        return;
    }
    for (i = 1; i < n - 1; i++) { dgl_tri(&st->s, pv[0], pv[i], pv[i + 1]); }
}

static void face(St *st, V3 a, V3 b, V3 c, uint32_t col, int wire) { V3 in[3]; V3 poly[CLIP_MAX]; int n; if (st->cull && backface(&st->cam, a, b, c)) { return; } in[0] = a; in[1] = b; in[2] = c; n = clip_near(&st->cam, in, 3, poly); draw_poly(st, poly, n, a, b, c, col, wire); }
static void quad(St *st, V3 a, V3 b, V3 c, V3 d, uint32_t col, int wire) { V3 in[4]; V3 poly[CLIP_MAX]; int n; if (st->cull && backface(&st->cam, a, b, c)) { return; } in[0] = a; in[1] = b; in[2] = c; in[3] = d; n = clip_near(&st->cam, in, 4, poly); draw_poly(st, poly, n, a, b, c, col, wire); }
static char *sw(char *p) { while (*p != '\0' && isspace((unsigned char)*p)) { p++; } return p; }
static int blank(const char *p) { while (*p != '\0' && isspace((unsigned char)*p)) { p++; } return *p == '\0' || *p == '\n' || *p == '#' || *p == ';' || (p[0] == '/' && p[1] == '/'); }
static int ri(const char *t, int *out) { char *e; long v; errno = 0; v = strtol(t, &e, 10); if (errno != 0 || e == t || (*sw(e) != '\0' && *sw(e) != '\n')) { return DGL_ERR_PARSE; } if (v < -2147483647L - 1L || v > 2147483647L) { return DGL_ERR_RANGE; } *out = (int)v; return DGL_OK; }
static int rf(const char *t, float *out) { char *e; float v; errno = 0; v = strtof(t, &e); if (errno != 0 || e == t || *e != '\0' || !isfinite(v)) { return DGL_ERR_PARSE; } *out = v; return DGL_OK; }
static int err(St *st, const char *m) { fprintf(stderr, "deadgl: %s:%d: %s\n", st->path, st->line, m); return DGL_ERR_PARSE; }
static int proof(const char *pp, const char *scene, const char *out, const DGL_Surface *s) { FILE *f; if (pp == NULL) { return DGL_OK; } f = fopen(pp, "wb"); if (f == NULL) { return DGL_ERR_IO; } fprintf(f, "DEADGL_PROOF\nversion %s\nscene %s\noutput %s\nwidth %d\nheight %d\npixels %llu\nhash %016llx\n", dgl_version(), scene, out ? out : "none", s->width, s->height, (unsigned long long)dgl_surface_pixel_count(s), (unsigned long long)dgl_hash(s)); return fclose(f) == 0 ? DGL_OK : DGL_ERR_IO; }
static int grid(DGL_Surface *s, int step, uint32_t col) { int x; int y; if (!dgl_surface_valid(s) || step <= 0) { return DGL_ERR_ARG; } for (x = 0; x < s->width; x += step) { dgl_line(s, x, 0, x, s->height - 1, col); } for (y = 0; y < s->height; y += step) { dgl_line(s, 0, y, s->width - 1, y, col); } return DGL_OK; }
static int color1(St *st, char **t, uint32_t *c) { if (t[1] == NULL || t[2] != NULL) { return DGL_ERR_PARSE; } if (dgl_parse_color(t[1], c) != DGL_OK) { return err(st, "bad color"); } return DGL_OK; }
static int i4c(St *st, char **t, int *a, int *b, int *c, int *d, uint32_t *col) { if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] != NULL) { return DGL_ERR_PARSE; } if (ri(t[1], a) != DGL_OK || ri(t[2], b) != DGL_OK || ri(t[3], c) != DGL_OK || ri(t[4], d) != DGL_OK) { return err(st, "bad integer"); } if (dgl_parse_color(t[5], col) != DGL_OK) { return err(st, "bad color"); } return DGL_OK; }
static int i3c(St *st, char **t, int *a, int *b, int *c, uint32_t *col) { if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] != NULL) { return DGL_ERR_PARSE; } if (ri(t[1], a) != DGL_OK || ri(t[2], b) != DGL_OK || ri(t[3], c) != DGL_OK) { return err(st, "bad integer"); } if (dgl_parse_color(t[4], col) != DGL_OK) { return err(st, "bad color"); } return DGL_OK; }
static int f6c(St *st, char **t, DGL_Vertex *a, DGL_Vertex *b, DGL_Vertex *c, uint32_t *col) { if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] == NULL || t[7] == NULL || t[8] != NULL) { return DGL_ERR_PARSE; } if (rf(t[1], &a->x) != DGL_OK || rf(t[2], &a->y) != DGL_OK || rf(t[3], &b->x) != DGL_OK || rf(t[4], &b->y) != DGL_OK || rf(t[5], &c->x) != DGL_OK || rf(t[6], &c->y) != DGL_OK) { return err(st, "bad float"); } if (dgl_parse_color(t[7], col) != DGL_OK) { return err(st, "bad color"); } a->z = 0.5f; b->z = 0.5f; c->z = 0.5f; a->color = *col; b->color = *col; c->color = *col; return DGL_OK; }
static int read_v3(char **t, int start, V3 *v) { return rf(t[start], &v->x) == DGL_OK && rf(t[start + 1], &v->y) == DGL_OK && rf(t[start + 2], &v->z) == DGL_OK; }
static int getv(St *st, int id, V3 *out) { if (id < 0 || id >= MAX_VERTS || !st->have[id]) { return 0; } *out = st->vert[id]; return 1; }
static int cmd_cam(St *st, char **t) { if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] == NULL || t[7] == NULL || t[8] != NULL) { return err(st, "camera wants: camera EX EY EZ TX TY TZ FOV"); } if (rf(t[1], &st->cam.eye.x) != DGL_OK || rf(t[2], &st->cam.eye.y) != DGL_OK || rf(t[3], &st->cam.eye.z) != DGL_OK || rf(t[4], &st->cam.target.x) != DGL_OK || rf(t[5], &st->cam.target.y) != DGL_OK || rf(t[6], &st->cam.target.z) != DGL_OK || rf(t[7], &st->cam.fov) != DGL_OK) { return err(st, "bad camera float"); } st->cam.up = v3(0.0f, 1.0f, 0.0f); st->cam.nearz = 0.05f; return DGL_OK; }
static int cmd_near(St *st, char **t) { float z; if (t[1] == NULL || t[2] != NULL) { return err(st, "near wants: near Z"); } if (rf(t[1], &z) != DGL_OK || z < 0.001f || z > 1000.0f) { return err(st, "bad near value"); } st->cam.nearz = z; return DGL_OK; }
static int cmd_light(St *st, char **t) { if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] != NULL) { return err(st, "light wants: light X Y Z"); } if (!read_v3(t, 1, &st->light)) { return err(st, "bad light float"); } if (!norm3(&st->light)) { return err(st, "bad light vector"); } return DGL_OK; }
static int cmd_shade(St *st, char **t) { if (t[1] == NULL || t[2] != NULL) { return err(st, "shade wants: shade flat|off"); } if (strcmp(t[1], "flat") == 0) { st->shade = 1; return DGL_OK; } if (strcmp(t[1], "off") == 0) { st->shade = 0; return DGL_OK; } return err(st, "bad shade mode"); }
static int cmd_vertex(St *st, char **t) { int id; V3 v; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] != NULL) { return err(st, "vertex wants: vertex ID X Y Z"); } if (ri(t[1], &id) != DGL_OK || id < 0 || id >= MAX_VERTS) { return err(st, "bad vertex id"); } if (!read_v3(t, 2, &v)) { return err(st, "bad vertex float"); } st->vert[id] = v; st->have[id] = 1u; return DGL_OK; }
static int cmd_face(St *st, char **t, int wire) { V3 a; V3 b; V3 c; uint32_t col; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] == NULL || t[7] == NULL || t[8] == NULL || t[9] == NULL || t[10] == NULL || t[11] != NULL) { return err(st, "face wants 9 numbers and a color"); } if (!read_v3(t, 1, &a) || !read_v3(t, 4, &b) || !read_v3(t, 7, &c)) { return err(st, "bad face float"); } if (dgl_parse_color(t[10], &col) != DGL_OK) { return err(st, "bad color"); } face(st, a, b, c, col, wire); return DGL_OK; }
static int cmd_quad(St *st, char **t, int wire) { V3 a; V3 b; V3 c; V3 d; uint32_t col; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] == NULL || t[7] == NULL || t[8] == NULL || t[9] == NULL || t[10] == NULL || t[11] == NULL || t[12] == NULL || t[13] == NULL || t[14] != NULL) { return err(st, "quad wants 12 numbers and a color"); } if (!read_v3(t, 1, &a) || !read_v3(t, 4, &b) || !read_v3(t, 7, &c) || !read_v3(t, 10, &d)) { return err(st, "bad quad float"); } if (dgl_parse_color(t[13], &col) != DGL_OK) { return err(st, "bad color"); } quad(st, a, b, c, d, col, wire); return DGL_OK; }
static int cmd_meshtri(St *st, char **t, int wire) { int ia; int ib; int ic; V3 a; V3 b; V3 c; uint32_t col; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] != NULL) { return err(st, "meshtri wants: meshtri A B C COLOR"); } if (ri(t[1], &ia) != DGL_OK || ri(t[2], &ib) != DGL_OK || ri(t[3], &ic) != DGL_OK || !getv(st, ia, &a) || !getv(st, ib, &b) || !getv(st, ic, &c)) { return err(st, "bad meshtri index"); } if (dgl_parse_color(t[4], &col) != DGL_OK) { return err(st, "bad color"); } face(st, a, b, c, col, wire); return DGL_OK; }
static int cmd_meshquad(St *st, char **t, int wire) { int ia; int ib; int ic; int id; V3 a; V3 b; V3 c; V3 d; uint32_t col; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] != NULL) { return err(st, "meshquad wants: meshquad A B C D COLOR"); } if (ri(t[1], &ia) != DGL_OK || ri(t[2], &ib) != DGL_OK || ri(t[3], &ic) != DGL_OK || ri(t[4], &id) != DGL_OK || !getv(st, ia, &a) || !getv(st, ib, &b) || !getv(st, ic, &c) || !getv(st, id, &d)) { return err(st, "bad meshquad index"); } if (dgl_parse_color(t[5], &col) != DGL_OK) { return err(st, "bad color"); } quad(st, a, b, c, d, col, wire); return DGL_OK; }
static int cmd_ztri(St *st, char **t) { DGL_Vertex a; DGL_Vertex b; DGL_Vertex c; uint32_t col; if (t[1] == NULL || t[2] == NULL || t[3] == NULL || t[4] == NULL || t[5] == NULL || t[6] == NULL || t[7] == NULL || t[8] == NULL || t[9] == NULL || t[10] == NULL || t[11] != NULL) { return err(st, "ztri wants 9 numbers and a color"); } if (rf(t[1], &a.x) != DGL_OK || rf(t[2], &a.y) != DGL_OK || rf(t[3], &a.z) != DGL_OK || rf(t[4], &b.x) != DGL_OK || rf(t[5], &b.y) != DGL_OK || rf(t[6], &b.z) != DGL_OK || rf(t[7], &c.x) != DGL_OK || rf(t[8], &c.y) != DGL_OK || rf(t[9], &c.z) != DGL_OK) { return err(st, "bad ztri float"); } if (dgl_parse_color(t[10], &col) != DGL_OK) { return err(st, "bad color"); } a.color = col; b.color = col; c.color = col; dgl_tri(&st->s, a, b, c); return DGL_OK; }
static int command(St *st, char **t) { uint32_t col; int a; int b; int c; int d; DGL_Vertex va; DGL_Vertex vb; DGL_Vertex vc; if (strcmp(t[0], "canvas") == 0) { DGL_Surface next = {0, 0, NULL, NULL}; if (t[1] == NULL || t[2] == NULL || t[3] != NULL) { return err(st, "canvas wants: canvas W H"); } if (ri(t[1], &a) != DGL_OK || ri(t[2], &b) != DGL_OK || dgl_surface_init(&next, a, b) != DGL_OK) { return err(st, "bad canvas"); } dgl_surface_free(&st->s); st->s = next; dgl_clear(&st->s, DGL_RGB(0, 0, 0)); dgl_clear_depth(&st->s, DGL_FAR); return DGL_OK; } if (strcmp(t[0], "camera") == 0) { return cmd_cam(st, t); } if (strcmp(t[0], "near") == 0) { return cmd_near(st, t); } if (strcmp(t[0], "light") == 0) { return cmd_light(st, t); } if (strcmp(t[0], "shade") == 0) { return cmd_shade(st, t); } if (strcmp(t[0], "vertex") == 0) { return cmd_vertex(st, t); } if (strcmp(t[0], "cull") == 0) { if (t[1] == NULL || t[2] != NULL) { return err(st, "cull wants: cull on|off"); } if (strcmp(t[1], "on") == 0) { st->cull = 1; return DGL_OK; } if (strcmp(t[1], "off") == 0) { st->cull = 0; return DGL_OK; } return err(st, "bad cull mode"); } if (strcmp(t[0], "clear") == 0) { if (color1(st, t, &col) != DGL_OK) { return err(st, "clear wants: clear COLOR"); } dgl_clear(&st->s, col); dgl_clear_depth(&st->s, DGL_FAR); return DGL_OK; } if (strcmp(t[0], "grid") == 0) { if (t[1] == NULL || t[2] == NULL || t[3] != NULL) { return err(st, "grid wants: grid STEP COLOR"); } if (ri(t[1], &a) != DGL_OK || dgl_parse_color(t[2], &col) != DGL_OK) { return err(st, "bad grid argument"); } return grid(&st->s, a, col); } if (strcmp(t[0], "line") == 0) { if (i4c(st, t, &a, &b, &c, &d, &col) != DGL_OK) { return err(st, "line wants: line X0 Y0 X1 Y1 COLOR"); } dgl_line(&st->s, a, b, c, d, col); return DGL_OK; } if (strcmp(t[0], "rect") == 0 || strcmp(t[0], "fillrect") == 0) { if (i4c(st, t, &a, &b, &c, &d, &col) != DGL_OK) { return err(st, "rect wants: rect X Y W H COLOR"); } if (strcmp(t[0], "rect") == 0) { dgl_rect(&st->s, a, b, c, d, col); } else { dgl_fill_rect(&st->s, a, b, c, d, col); } return DGL_OK; } if (strcmp(t[0], "circle") == 0 || strcmp(t[0], "fillcircle") == 0) { if (i3c(st, t, &a, &b, &c, &col) != DGL_OK) { return err(st, "circle wants: circle X Y R COLOR"); } if (strcmp(t[0], "circle") == 0) { dgl_circle(&st->s, a, b, c, col); } else { dgl_fill_circle(&st->s, a, b, c, col); } return DGL_OK; } if (strcmp(t[0], "tri") == 0 || strcmp(t[0], "wiretri") == 0) { if (f6c(st, t, &va, &vb, &vc, &col) != DGL_OK) { return err(st, "tri wants: tri X0 Y0 X1 Y1 X2 Y2 COLOR"); } if (strcmp(t[0], "tri") == 0) { dgl_tri(&st->s, va, vb, vc); } else { dgl_wire_tri(&st->s, va, vb, vc, col); } return DGL_OK; } if (strcmp(t[0], "ztri") == 0) { return cmd_ztri(st, t); } if (strcmp(t[0], "face") == 0) { return cmd_face(st, t, 0); } if (strcmp(t[0], "wireface") == 0) { return cmd_face(st, t, 1); } if (strcmp(t[0], "quad") == 0) { return cmd_quad(st, t, 0); } if (strcmp(t[0], "wirequad") == 0) { return cmd_quad(st, t, 1); } if (strcmp(t[0], "meshtri") == 0) { return cmd_meshtri(st, t, 0); } if (strcmp(t[0], "meshwiretri") == 0) { return cmd_meshtri(st, t, 1); } if (strcmp(t[0], "meshquad") == 0) { return cmd_meshquad(st, t, 0); } if (strcmp(t[0], "meshwirequad") == 0) { return cmd_meshquad(st, t, 1); } return err(st, "unknown command"); }
static int run_scene(const char *path, const char *out, const char *pp, int hash_only) { FILE *f; St st; char line[LINE_MAX_DGL]; int rc = DGL_OK; memset(&st, 0, sizeof(st)); st.path = path; st.cam = default_cam(); st.light = v3(0.4f, 1.0f, 0.7f); (void)norm3(&st.light); if (dgl_surface_init(&st.s, 640, 360) != DGL_OK) { return 1; } dgl_clear(&st.s, DGL_RGB(0, 0, 0)); f = fopen(path, "r"); if (f == NULL) { fprintf(stderr, "deadgl: cannot open %s\n", path); dgl_surface_free(&st.s); return 1; } while (fgets(line, sizeof(line), f) != NULL) { char *tok[TOK_MAX]; char *p; int n = 0; st.line++; if (blank(line)) { continue; } if (strchr(line, '\n') == NULL && !feof(f)) { rc = err(&st, "line too long"); break; } p = strtok(line, " \t\r\n"); while (p != NULL && n < TOK_MAX - 1) { tok[n++] = p; p = strtok(NULL, " \t\r\n"); } tok[n] = NULL; if (p != NULL) { rc = err(&st, "too many tokens"); break; } if (n > 0 && command(&st, tok) != DGL_OK) { rc = DGL_ERR_PARSE; break; } } fclose(f); if (rc == DGL_OK) { if (hash_only) { printf("%016llx  %s\n", (unsigned long long)dgl_hash(&st.s), path); } else if (dgl_save_ppm(&st.s, out) != DGL_OK) { fprintf(stderr, "deadgl: cannot write %s\n", out); rc = DGL_ERR_IO; } else if (proof(pp, path, out, &st.s) != DGL_OK) { fprintf(stderr, "deadgl: cannot write proof %s\n", pp); rc = DGL_ERR_IO; } } dgl_surface_free(&st.s); return rc == DGL_OK ? 0 : 1; }
static void cube(DGL_Surface *s) { St st; uint32_t w = DGL_RGB(240, 240, 216); memset(&st, 0, sizeof(st)); st.s = *s; st.path = "demo"; st.cam = default_cam(); st.cam.eye = v3(3.0f, 2.2f, 4.2f); st.cam.target = v3(0.0f, 0.0f, 0.0f); st.cam.fov = 62.0f; st.cam.nearz = 0.05f; st.cull = 1; st.shade = 1; st.light = v3(0.4f, 1.0f, 0.7f); (void)norm3(&st.light); st.vert[0] = v3(-1.0f, -1.0f, -1.0f); st.vert[1] = v3(1.0f, -1.0f, -1.0f); st.vert[2] = v3(1.0f, 1.0f, -1.0f); st.vert[3] = v3(-1.0f, 1.0f, -1.0f); st.vert[4] = v3(-1.0f, -1.0f, 1.0f); st.vert[5] = v3(1.0f, -1.0f, 1.0f); st.vert[6] = v3(1.0f, 1.0f, 1.0f); st.vert[7] = v3(-1.0f, 1.0f, 1.0f); quad(&st, st.vert[4], st.vert[5], st.vert[6], st.vert[7], DGL_RGB(255, 136, 34), 0); quad(&st, st.vert[5], st.vert[1], st.vert[2], st.vert[6], DGL_RGB(42, 150, 255), 0); quad(&st, st.vert[7], st.vert[6], st.vert[2], st.vert[3], DGL_RGB(0, 255, 153), 0); quad(&st, st.vert[1], st.vert[0], st.vert[3], st.vert[2], DGL_RGB(20, 45, 80), 0); quad(&st, st.vert[0], st.vert[4], st.vert[7], st.vert[3], DGL_RGB(40, 70, 95), 0); quad(&st, st.vert[0], st.vert[1], st.vert[5], st.vert[4], DGL_RGB(45, 40, 60), 0); quad(&st, st.vert[4], st.vert[5], st.vert[6], st.vert[7], w, 1); quad(&st, st.vert[5], st.vert[1], st.vert[2], st.vert[6], w, 1); quad(&st, st.vert[7], st.vert[6], st.vert[2], st.vert[3], w, 1); quad(&st, st.vert[1], st.vert[0], st.vert[3], st.vert[2], w, 1); quad(&st, st.vert[0], st.vert[4], st.vert[7], st.vert[3], w, 1); quad(&st, st.vert[0], st.vert[1], st.vert[5], st.vert[4], w, 1); *s = st.s; }
static int demo(const char *name, const char *out) { DGL_Surface s; DGL_Vertex a; DGL_Vertex b; DGL_Vertex c; int rc = dgl_surface_init(&s, 640, 360); if (rc != DGL_OK) { fprintf(stderr, "deadgl: surface init failed: %s\n", dgl_result_name(rc)); return 1; } dgl_clear(&s, DGL_RGB(5, 6, 8)); dgl_clear_depth(&s, DGL_FAR); (void)grid(&s, 32, DGL_RGB(20, 24, 32)); if (strcmp(name, "depth") == 0) { a = (DGL_Vertex){120.0f, 60.0f, 0.8f, DGL_RGB(120, 50, 255)}; b = (DGL_Vertex){560.0f, 300.0f, 0.8f, DGL_RGB(120, 50, 255)}; c = (DGL_Vertex){80.0f, 310.0f, 0.8f, DGL_RGB(120, 50, 255)}; dgl_tri(&s, a, b, c); a = (DGL_Vertex){520.0f, 50.0f, 0.2f, DGL_RGB(255, 180, 50)}; b = (DGL_Vertex){560.0f, 310.0f, 0.2f, DGL_RGB(255, 180, 50)}; c = (DGL_Vertex){160.0f, 260.0f, 0.2f, DGL_RGB(255, 180, 50)}; dgl_tri(&s, a, b, c); } else if (strcmp(name, "shrine") == 0) { a = (DGL_Vertex){320.0f, 36.0f, 0.5f, DGL_RGB(255, 204, 85)}; b = (DGL_Vertex){96.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)}; c = (DGL_Vertex){544.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)}; dgl_tri(&s, a, b, c); dgl_wire_tri(&s, a, b, c, DGL_RGB(240, 240, 216)); dgl_fill_circle(&s, 320, 180, 22, DGL_RGB(0, 255, 153)); dgl_circle(&s, 320, 180, 46, DGL_RGB(232, 232, 216)); dgl_rect(&s, 40, 32, 560, 296, DGL_RGB(232, 232, 216)); } else if (strcmp(name, "cube") == 0) { cube(&s); } else { fprintf(stderr, "deadgl: unknown demo %s\n", name); dgl_surface_free(&s); return 1; } printf("hash %016llx\n", (unsigned long long)dgl_hash(&s)); rc = dgl_save_ppm(&s, out); dgl_surface_free(&s); return rc == DGL_OK ? 0 : 1; }
static void usage(void) { puts("DEADGL " DGL_VERSION); puts("usage:"); puts("  deadgl --version"); puts("  deadgl demo shrine -o out.ppm"); puts("  deadgl demo depth -o out.ppm"); puts("  deadgl demo cube -o out.ppm"); puts("  deadgl run scene.dgl -o out.ppm"); puts("  deadgl prove scene.dgl -o out.ppm -p out.proof"); puts("  deadgl hash scene.dgl"); }
int main(int argc, char **argv) { if (argc == 2 && strcmp(argv[1], "--version") == 0) { puts("DEADGL " DGL_VERSION); return 0; } if (argc == 5 && strcmp(argv[1], "demo") == 0 && strcmp(argv[3], "-o") == 0) { return demo(argv[2], argv[4]); } if (argc == 5 && strcmp(argv[1], "run") == 0 && strcmp(argv[3], "-o") == 0) { return run_scene(argv[2], argv[4], NULL, 0); } if (argc == 7 && strcmp(argv[1], "prove") == 0 && strcmp(argv[3], "-o") == 0 && strcmp(argv[5], "-p") == 0) { return run_scene(argv[2], argv[4], argv[6], 0); } if (argc == 3 && strcmp(argv[1], "hash") == 0) { return run_scene(argv[2], "", NULL, 1); } usage(); return argc == 1 ? 0 : 1; }
