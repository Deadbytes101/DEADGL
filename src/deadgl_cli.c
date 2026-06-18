#include "deadgl.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DGL_LINE_MAX 1024
#define DGL_TOKEN_MAX 32
#define DGL_PI 3.14159265358979323846

typedef struct DGL_CliVec3 {
    float x;
    float y;
    float z;
} DGL_CliVec3;

typedef struct DGL_CliCamera {
    DGL_CliVec3 eye;
    DGL_CliVec3 target;
    DGL_CliVec3 up;
    float fov_degrees;
    float near_z;
} DGL_CliCamera;

typedef struct SceneState {
    DGL_Surface surface;
    const char *path;
    int line_no;
    DGL_CliCamera camera;
} SceneState;

static DGL_CliVec3 vec3(float x, float y, float z) {
    DGL_CliVec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

static DGL_CliVec3 vec3_sub(DGL_CliVec3 a, DGL_CliVec3 b) {
    return vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static double vec3_dot(DGL_CliVec3 a, DGL_CliVec3 b) {
    return (double)a.x * (double)b.x + (double)a.y * (double)b.y + (double)a.z * (double)b.z;
}

static DGL_CliVec3 vec3_cross(DGL_CliVec3 a, DGL_CliVec3 b) {
    return vec3(
        (float)((double)a.y * (double)b.z - (double)a.z * (double)b.y),
        (float)((double)a.z * (double)b.x - (double)a.x * (double)b.z),
        (float)((double)a.x * (double)b.y - (double)a.y * (double)b.x));
}

static int vec3_normalize(DGL_CliVec3 *v) {
    double len;
    if (v == NULL) {
        return 0;
    }
    len = sqrt(vec3_dot(*v, *v));
    if (!isfinite(len) || len < 1.0e-9) {
        return 0;
    }
    v->x = (float)((double)v->x / len);
    v->y = (float)((double)v->y / len);
    v->z = (float)((double)v->z / len);
    return 1;
}

static DGL_CliCamera camera_default(void) {
    DGL_CliCamera camera;
    camera.eye = vec3(3.0f, 2.0f, 5.0f);
    camera.target = vec3(0.0f, 0.0f, 0.0f);
    camera.up = vec3(0.0f, 1.0f, 0.0f);
    camera.fov_degrees = 70.0f;
    camera.near_z = 0.05f;
    return camera;
}

static int project_vertex(const DGL_Surface *s, const DGL_CliCamera *camera, DGL_CliVec3 p, uint32_t color, DGL_Vertex *out) {
    DGL_CliVec3 forward;
    DGL_CliVec3 right;
    DGL_CliVec3 up;
    DGL_CliVec3 rel;
    double cx;
    double cy;
    double cz;
    double near_z;
    double fov;
    double scale;
    double sx;
    double sy;

    if (!dgl_surface_valid(s) || camera == NULL || out == NULL) {
        return 0;
    }

    forward = vec3_sub(camera->target, camera->eye);
    if (!vec3_normalize(&forward)) {
        return 0;
    }

    up = camera->up;
    if (!vec3_normalize(&up)) {
        up = vec3(0.0f, 1.0f, 0.0f);
    }

    right = vec3_cross(forward, up);
    if (!vec3_normalize(&right)) {
        right = vec3(1.0f, 0.0f, 0.0f);
    }

    up = vec3_cross(right, forward);
    if (!vec3_normalize(&up)) {
        return 0;
    }

    rel = vec3_sub(p, camera->eye);
    cx = vec3_dot(rel, right);
    cy = vec3_dot(rel, up);
    cz = vec3_dot(rel, forward);
    near_z = camera->near_z > 0.0f ? (double)camera->near_z : 0.05;
    if (!isfinite(cz) || cz <= near_z) {
        return 0;
    }

    fov = camera->fov_degrees > 1.0f ? (double)camera->fov_degrees : 70.0;
    if (fov > 170.0) {
        fov = 170.0;
    }

    scale = ((double)s->height * 0.5) / tan((fov * DGL_PI / 180.0) * 0.5);
    sx = (((double)s->width - 1.0) * 0.5) + (cx * scale / cz);
    sy = (((double)s->height - 1.0) * 0.5) - (cy * scale / cz);
    if (!isfinite(sx) || !isfinite(sy)) {
        return 0;
    }

    out->x = (float)sx;
    out->y = (float)sy;
    out->z = (float)cz;
    out->color = color;
    return 1;
}

static void draw_face(DGL_Surface *s, const DGL_CliCamera *camera, DGL_CliVec3 a, DGL_CliVec3 b, DGL_CliVec3 c, uint32_t color) {
    DGL_Vertex va;
    DGL_Vertex vb;
    DGL_Vertex vc;
    if (project_vertex(s, camera, a, color, &va) && project_vertex(s, camera, b, color, &vb) && project_vertex(s, camera, c, color, &vc)) {
        dgl_tri(s, va, vb, vc);
    }
}

static void draw_wire_face(DGL_Surface *s, const DGL_CliCamera *camera, DGL_CliVec3 a, DGL_CliVec3 b, DGL_CliVec3 c, uint32_t color) {
    DGL_Vertex va;
    DGL_Vertex vb;
    DGL_Vertex vc;
    if (project_vertex(s, camera, a, color, &va) && project_vertex(s, camera, b, color, &vb) && project_vertex(s, camera, c, color, &vc)) {
        dgl_wire_tri(s, va, vb, vc, color);
    }
}

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

static int read_i(const char *text, int *out) {
    char *end;
    long v;
    errno = 0;
    v = strtol(text, &end, 10);
    if (errno != 0 || end == text || (*skip_ws(end) != '\0' && *skip_ws(end) != '\n')) {
        return DGL_ERR_PARSE;
    }
    if (v < -2147483647L - 1L || v > 2147483647L) {
        return DGL_ERR_RANGE;
    }
    *out = (int)v;
    return DGL_OK;
}

static int read_f(const char *text, float *out) {
    char *end;
    float v;
    errno = 0;
    v = strtof(text, &end);
    if (errno != 0 || end == text || *end != '\0' || !isfinite(v)) {
        return DGL_ERR_PARSE;
    }
    *out = v;
    return DGL_OK;
}

static int scene_error(const SceneState *st, const char *msg) {
    fprintf(stderr, "deadgl: %s:%d: %s\n", st->path, st->line_no, msg);
    return DGL_ERR_PARSE;
}

static int write_proof_file(const char *proof_path, const char *scene_path, const char *out_path, const DGL_Surface *s) {
    FILE *f;
    if (proof_path == NULL) {
        return DGL_OK;
    }
    f = fopen(proof_path, "wb");
    if (f == NULL) {
        return DGL_ERR_IO;
    }
    fprintf(f, "DEADGL_PROOF\n");
    fprintf(f, "version %s\n", dgl_version());
    fprintf(f, "scene %s\n", scene_path);
    fprintf(f, "output %s\n", out_path == NULL ? "none" : out_path);
    fprintf(f, "width %d\n", s->width);
    fprintf(f, "height %d\n", s->height);
    fprintf(f, "pixels %llu\n", (unsigned long long)dgl_surface_pixel_count(s));
    fprintf(f, "hash %016llx\n", (unsigned long long)dgl_hash(s));
    if (fclose(f) != 0) {
        return DGL_ERR_IO;
    }
    return DGL_OK;
}

static int draw_grid(DGL_Surface *s, int step, uint32_t color) {
    int x;
    int y;
    if (!dgl_surface_valid(s) || step <= 0) {
        return DGL_ERR_ARG;
    }
    for (x = 0; x < s->width; x += step) {
        dgl_line(s, x, 0, x, s->height - 1, color);
    }
    for (y = 0; y < s->height; y += step) {
        dgl_line(s, 0, y, s->width - 1, y, color);
    }
    return DGL_OK;
}

static int scene_canvas(SceneState *st, char **tok) {
    int w;
    int h;
    DGL_Surface next = {0, 0, NULL, NULL};
    if (tok[1] == NULL || tok[2] == NULL || tok[3] != NULL) {
        return scene_error(st, "canvas wants: canvas W H");
    }
    if (read_i(tok[1], &w) != DGL_OK || read_i(tok[2], &h) != DGL_OK) {
        return scene_error(st, "bad canvas integer");
    }
    if (dgl_surface_init(&next, w, h) != DGL_OK) {
        return scene_error(st, "canvas allocation failed or size rejected");
    }
    dgl_surface_free(&st->surface);
    st->surface = next;
    dgl_clear(&st->surface, DGL_RGB(0, 0, 0));
    dgl_clear_depth(&st->surface, DGL_FAR);
    return DGL_OK;
}

static int scene_color1(SceneState *st, char **tok, uint32_t *color) {
    if (tok[1] == NULL || tok[2] != NULL) {
        return DGL_ERR_PARSE;
    }
    if (dgl_parse_color(tok[1], color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    return DGL_OK;
}

static int scene_i4_color(SceneState *st, char **tok, int *a, int *b, int *c, int *d, uint32_t *color) {
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] == NULL || tok[6] != NULL) {
        return DGL_ERR_PARSE;
    }
    if (read_i(tok[1], a) != DGL_OK || read_i(tok[2], b) != DGL_OK || read_i(tok[3], c) != DGL_OK || read_i(tok[4], d) != DGL_OK) {
        return scene_error(st, "bad integer");
    }
    if (dgl_parse_color(tok[5], color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    return DGL_OK;
}

static int scene_i3_color(SceneState *st, char **tok, int *a, int *b, int *c, uint32_t *color) {
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] != NULL) {
        return DGL_ERR_PARSE;
    }
    if (read_i(tok[1], a) != DGL_OK || read_i(tok[2], b) != DGL_OK || read_i(tok[3], c) != DGL_OK) {
        return scene_error(st, "bad integer");
    }
    if (dgl_parse_color(tok[4], color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    return DGL_OK;
}

static int scene_f6_color(SceneState *st, char **tok, DGL_Vertex *a, DGL_Vertex *b, DGL_Vertex *c, uint32_t *color) {
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] == NULL || tok[6] == NULL || tok[7] == NULL || tok[8] != NULL) {
        return DGL_ERR_PARSE;
    }
    if (read_f(tok[1], &a->x) != DGL_OK || read_f(tok[2], &a->y) != DGL_OK || read_f(tok[3], &b->x) != DGL_OK || read_f(tok[4], &b->y) != DGL_OK || read_f(tok[5], &c->x) != DGL_OK || read_f(tok[6], &c->y) != DGL_OK) {
        return scene_error(st, "bad float");
    }
    if (dgl_parse_color(tok[7], color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    a->z = 0.5f;
    b->z = 0.5f;
    c->z = 0.5f;
    a->color = *color;
    b->color = *color;
    c->color = *color;
    return DGL_OK;
}

static int scene_camera(SceneState *st, char **tok) {
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] == NULL || tok[6] == NULL || tok[7] == NULL || tok[8] != NULL) {
        return scene_error(st, "camera wants: camera EX EY EZ TX TY TZ FOV");
    }
    if (read_f(tok[1], &st->camera.eye.x) != DGL_OK || read_f(tok[2], &st->camera.eye.y) != DGL_OK || read_f(tok[3], &st->camera.eye.z) != DGL_OK || read_f(tok[4], &st->camera.target.x) != DGL_OK || read_f(tok[5], &st->camera.target.y) != DGL_OK || read_f(tok[6], &st->camera.target.z) != DGL_OK || read_f(tok[7], &st->camera.fov_degrees) != DGL_OK) {
        return scene_error(st, "bad camera float");
    }
    st->camera.up = vec3(0.0f, 1.0f, 0.0f);
    st->camera.near_z = 0.05f;
    return DGL_OK;
}

static int scene_face(SceneState *st, char **tok, int wire) {
    DGL_CliVec3 a;
    DGL_CliVec3 b;
    DGL_CliVec3 c;
    uint32_t color;
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] == NULL || tok[6] == NULL || tok[7] == NULL || tok[8] == NULL || tok[9] == NULL || tok[10] == NULL || tok[11] != NULL) {
        return scene_error(st, "face wants 9 numbers and a color");
    }
    if (read_f(tok[1], &a.x) != DGL_OK || read_f(tok[2], &a.y) != DGL_OK || read_f(tok[3], &a.z) != DGL_OK || read_f(tok[4], &b.x) != DGL_OK || read_f(tok[5], &b.y) != DGL_OK || read_f(tok[6], &b.z) != DGL_OK || read_f(tok[7], &c.x) != DGL_OK || read_f(tok[8], &c.y) != DGL_OK || read_f(tok[9], &c.z) != DGL_OK) {
        return scene_error(st, "bad face float");
    }
    if (dgl_parse_color(tok[10], &color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    if (wire) {
        draw_wire_face(&st->surface, &st->camera, a, b, c, color);
    } else {
        draw_face(&st->surface, &st->camera, a, b, c, color);
    }
    return DGL_OK;
}

static int scene_ztri(SceneState *st, char **tok) {
    DGL_Vertex a;
    DGL_Vertex b;
    DGL_Vertex c;
    uint32_t color;
    if (tok[1] == NULL || tok[2] == NULL || tok[3] == NULL || tok[4] == NULL || tok[5] == NULL || tok[6] == NULL || tok[7] == NULL || tok[8] == NULL || tok[9] == NULL || tok[10] == NULL || tok[11] != NULL) {
        return scene_error(st, "ztri wants 9 numbers and a color");
    }
    if (read_f(tok[1], &a.x) != DGL_OK || read_f(tok[2], &a.y) != DGL_OK || read_f(tok[3], &a.z) != DGL_OK || read_f(tok[4], &b.x) != DGL_OK || read_f(tok[5], &b.y) != DGL_OK || read_f(tok[6], &b.z) != DGL_OK || read_f(tok[7], &c.x) != DGL_OK || read_f(tok[8], &c.y) != DGL_OK || read_f(tok[9], &c.z) != DGL_OK) {
        return scene_error(st, "bad ztri float");
    }
    if (dgl_parse_color(tok[10], &color) != DGL_OK) {
        return scene_error(st, "bad color");
    }
    a.color = color;
    b.color = color;
    c.color = color;
    dgl_tri(&st->surface, a, b, c);
    return DGL_OK;
}

static int scene_command(SceneState *st, char **tok) {
    uint32_t color;
    int a;
    int b;
    int c;
    int d;
    DGL_Vertex va;
    DGL_Vertex vb;
    DGL_Vertex vc;

    if (strcmp(tok[0], "canvas") == 0) {
        return scene_canvas(st, tok);
    }
    if (strcmp(tok[0], "camera") == 0) {
        return scene_camera(st, tok);
    }
    if (strcmp(tok[0], "clear") == 0) {
        if (scene_color1(st, tok, &color) != DGL_OK) {
            return scene_error(st, "clear wants: clear COLOR");
        }
        dgl_clear(&st->surface, color);
        dgl_clear_depth(&st->surface, DGL_FAR);
        return DGL_OK;
    }
    if (strcmp(tok[0], "grid") == 0) {
        if (tok[1] == NULL || tok[2] == NULL || tok[3] != NULL) {
            return scene_error(st, "grid wants: grid STEP COLOR");
        }
        if (read_i(tok[1], &a) != DGL_OK || dgl_parse_color(tok[2], &color) != DGL_OK) {
            return scene_error(st, "bad grid argument");
        }
        return draw_grid(&st->surface, a, color);
    }
    if (strcmp(tok[0], "line") == 0) {
        if (scene_i4_color(st, tok, &a, &b, &c, &d, &color) != DGL_OK) {
            return scene_error(st, "line wants: line X0 Y0 X1 Y1 COLOR");
        }
        dgl_line(&st->surface, a, b, c, d, color);
        return DGL_OK;
    }
    if (strcmp(tok[0], "rect") == 0 || strcmp(tok[0], "fillrect") == 0) {
        if (scene_i4_color(st, tok, &a, &b, &c, &d, &color) != DGL_OK) {
            return scene_error(st, "rect wants: rect X Y W H COLOR");
        }
        if (strcmp(tok[0], "rect") == 0) {
            dgl_rect(&st->surface, a, b, c, d, color);
        } else {
            dgl_fill_rect(&st->surface, a, b, c, d, color);
        }
        return DGL_OK;
    }
    if (strcmp(tok[0], "circle") == 0 || strcmp(tok[0], "fillcircle") == 0) {
        if (scene_i3_color(st, tok, &a, &b, &c, &color) != DGL_OK) {
            return scene_error(st, "circle wants: circle X Y R COLOR");
        }
        if (strcmp(tok[0], "circle") == 0) {
            dgl_circle(&st->surface, a, b, c, color);
        } else {
            dgl_fill_circle(&st->surface, a, b, c, color);
        }
        return DGL_OK;
    }
    if (strcmp(tok[0], "tri") == 0 || strcmp(tok[0], "wiretri") == 0) {
        if (scene_f6_color(st, tok, &va, &vb, &vc, &color) != DGL_OK) {
            return scene_error(st, "tri wants: tri X0 Y0 X1 Y1 X2 Y2 COLOR");
        }
        if (strcmp(tok[0], "tri") == 0) {
            dgl_tri(&st->surface, va, vb, vc);
        } else {
            dgl_wire_tri(&st->surface, va, vb, vc, color);
        }
        return DGL_OK;
    }
    if (strcmp(tok[0], "ztri") == 0) {
        return scene_ztri(st, tok);
    }
    if (strcmp(tok[0], "face") == 0) {
        return scene_face(st, tok, 0);
    }
    if (strcmp(tok[0], "wireface") == 0) {
        return scene_face(st, tok, 1);
    }
    return scene_error(st, "unknown command");
}

static int run_scene(const char *path, const char *out, const char *proof_path, int hash_only) {
    FILE *f;
    SceneState st;
    char line[DGL_LINE_MAX];
    int rc = DGL_OK;

    st.surface.width = 0;
    st.surface.height = 0;
    st.surface.pixels = NULL;
    st.surface.depth = NULL;
    st.path = path;
    st.line_no = 0;
    st.camera = camera_default();

    if (dgl_surface_init(&st.surface, 640, 360) != DGL_OK) {
        return 1;
    }
    dgl_clear(&st.surface, DGL_RGB(0, 0, 0));

    f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "deadgl: cannot open %s\n", path);
        dgl_surface_free(&st.surface);
        return 1;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        char *tok[DGL_TOKEN_MAX];
        char *p;
        int n = 0;
        st.line_no++;
        if (blank_or_comment(line)) {
            continue;
        }
        if (strchr(line, '\n') == NULL && !feof(f)) {
            rc = scene_error(&st, "line too long");
            break;
        }
        p = strtok(line, " \t\r\n");
        while (p != NULL && n < DGL_TOKEN_MAX - 1) {
            tok[n++] = p;
            p = strtok(NULL, " \t\r\n");
        }
        tok[n] = NULL;
        if (p != NULL) {
            rc = scene_error(&st, "too many tokens");
            break;
        }
        if (n > 0 && scene_command(&st, tok) != DGL_OK) {
            rc = DGL_ERR_PARSE;
            break;
        }
    }
    fclose(f);

    if (rc == DGL_OK) {
        if (hash_only) {
            printf("%016llx  %s\n", (unsigned long long)dgl_hash(&st.surface), path);
        } else if (dgl_save_ppm(&st.surface, out) != DGL_OK) {
            fprintf(stderr, "deadgl: cannot write %s\n", out);
            rc = DGL_ERR_IO;
        } else if (write_proof_file(proof_path, path, out, &st.surface) != DGL_OK) {
            fprintf(stderr, "deadgl: cannot write proof %s\n", proof_path);
            rc = DGL_ERR_IO;
        }
    }
    dgl_surface_free(&st.surface);
    return rc == DGL_OK ? 0 : 1;
}

static void demo_cube(DGL_Surface *s) {
    DGL_CliCamera camera = camera_default();
    DGL_CliVec3 n0 = vec3(-1.0f, -1.0f, -1.0f);
    DGL_CliVec3 n1 = vec3(1.0f, -1.0f, -1.0f);
    DGL_CliVec3 n2 = vec3(1.0f, 1.0f, -1.0f);
    DGL_CliVec3 n3 = vec3(-1.0f, 1.0f, -1.0f);
    DGL_CliVec3 f0 = vec3(-1.0f, -1.0f, 1.0f);
    DGL_CliVec3 f1 = vec3(1.0f, -1.0f, 1.0f);
    DGL_CliVec3 f2 = vec3(1.0f, 1.0f, 1.0f);
    DGL_CliVec3 f3 = vec3(-1.0f, 1.0f, 1.0f);
    camera.eye = vec3(3.0f, 2.2f, 4.2f);
    camera.target = vec3(0.0f, 0.0f, 0.0f);
    camera.fov_degrees = 62.0f;

    draw_face(s, &camera, f0, f1, f2, DGL_RGB(255, 136, 34));
    draw_face(s, &camera, f0, f2, f3, DGL_RGB(255, 136, 34));
    draw_face(s, &camera, n0, n2, n1, DGL_RGB(42, 150, 255));
    draw_face(s, &camera, n0, n3, n2, DGL_RGB(42, 150, 255));
    draw_face(s, &camera, n3, f3, f2, DGL_RGB(0, 255, 153));
    draw_face(s, &camera, n3, f2, n2, DGL_RGB(0, 255, 153));
    draw_wire_face(s, &camera, f0, f1, f2, DGL_RGB(240, 240, 216));
    draw_wire_face(s, &camera, f0, f2, f3, DGL_RGB(240, 240, 216));
    draw_wire_face(s, &camera, n0, n2, n1, DGL_RGB(240, 240, 216));
    draw_wire_face(s, &camera, n0, n3, n2, DGL_RGB(240, 240, 216));
    draw_wire_face(s, &camera, n3, f3, f2, DGL_RGB(240, 240, 216));
    draw_wire_face(s, &camera, n3, f2, n2, DGL_RGB(240, 240, 216));
}

static int demo(const char *name, const char *out) {
    DGL_Surface s;
    DGL_Vertex a;
    DGL_Vertex b;
    DGL_Vertex c;
    int rc;

    rc = dgl_surface_init(&s, 640, 360);
    if (rc != DGL_OK) {
        fprintf(stderr, "deadgl: surface init failed: %s\n", dgl_result_name(rc));
        return 1;
    }
    dgl_clear(&s, DGL_RGB(5, 6, 8));
    dgl_clear_depth(&s, DGL_FAR);
    draw_grid(&s, 32, DGL_RGB(20, 24, 32));

    if (strcmp(name, "depth") == 0) {
        a = (DGL_Vertex){120.0f, 60.0f, 0.8f, DGL_RGB(120, 50, 255)};
        b = (DGL_Vertex){560.0f, 300.0f, 0.8f, DGL_RGB(120, 50, 255)};
        c = (DGL_Vertex){80.0f, 310.0f, 0.8f, DGL_RGB(120, 50, 255)};
        dgl_tri(&s, a, b, c);
        a = (DGL_Vertex){520.0f, 50.0f, 0.2f, DGL_RGB(255, 180, 50)};
        b = (DGL_Vertex){560.0f, 310.0f, 0.2f, DGL_RGB(255, 180, 50)};
        c = (DGL_Vertex){160.0f, 260.0f, 0.2f, DGL_RGB(255, 180, 50)};
        dgl_tri(&s, a, b, c);
    } else if (strcmp(name, "shrine") == 0) {
        a = (DGL_Vertex){320.0f, 36.0f, 0.5f, DGL_RGB(255, 204, 85)};
        b = (DGL_Vertex){96.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)};
        c = (DGL_Vertex){544.0f, 304.0f, 0.5f, DGL_RGB(255, 204, 85)};
        dgl_tri(&s, a, b, c);
        dgl_wire_tri(&s, a, b, c, DGL_RGB(240, 240, 216));
        dgl_fill_circle(&s, 320, 180, 22, DGL_RGB(0, 255, 153));
        dgl_circle(&s, 320, 180, 46, DGL_RGB(232, 232, 216));
        dgl_rect(&s, 40, 32, 560, 296, DGL_RGB(232, 232, 216));
    } else if (strcmp(name, "cube") == 0) {
        demo_cube(&s);
    } else {
        fprintf(stderr, "deadgl: unknown demo %s\n", name);
        dgl_surface_free(&s);
        return 1;
    }

    printf("hash %016llx\n", (unsigned long long)dgl_hash(&s));
    rc = dgl_save_ppm(&s, out);
    dgl_surface_free(&s);
    if (rc != DGL_OK) {
        fprintf(stderr, "deadgl: write failed: %s\n", dgl_result_name(rc));
        return 1;
    }
    return 0;
}

static void usage(void) {
    puts("DEADGL " DGL_VERSION);
    puts("usage:");
    puts("  deadgl --version");
    puts("  deadgl demo shrine -o out.ppm");
    puts("  deadgl demo depth -o out.ppm");
    puts("  deadgl demo cube -o out.ppm");
    puts("  deadgl run scene.dgl -o out.ppm");
    puts("  deadgl prove scene.dgl -o out.ppm -p out.proof");
    puts("  deadgl hash scene.dgl");
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        puts("DEADGL " DGL_VERSION);
        return 0;
    }
    if (argc == 5 && strcmp(argv[1], "demo") == 0 && strcmp(argv[3], "-o") == 0) {
        return demo(argv[2], argv[4]);
    }
    if (argc == 5 && strcmp(argv[1], "run") == 0 && strcmp(argv[3], "-o") == 0) {
        return run_scene(argv[2], argv[4], NULL, 0);
    }
    if (argc == 7 && strcmp(argv[1], "prove") == 0 && strcmp(argv[3], "-o") == 0 && strcmp(argv[5], "-p") == 0) {
        return run_scene(argv[2], argv[4], argv[6], 0);
    }
    if (argc == 3 && strcmp(argv[1], "hash") == 0) {
        return run_scene(argv[2], "", NULL, 1);
    }
    usage();
    return argc == 1 ? 0 : 1;
}
