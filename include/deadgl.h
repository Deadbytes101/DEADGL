#ifndef DEADGL_H
#define DEADGL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DGL_VERSION "3.2.0"
#define DGL_MAX_DIM 16384
#define DGL_MAX_PIXELS 67108864u
#define DGL_FAR 1.0e30f
#define DGL_ARGB(a,r,g,b) ((((uint32_t)(a) & 255u) << 24) | (((uint32_t)(r) & 255u) << 16) | (((uint32_t)(g) & 255u) << 8) | ((uint32_t)(b) & 255u))
#define DGL_RGB(r,g,b) DGL_ARGB(255u, (r), (g), (b))

typedef enum DGL_Result {
    DGL_OK = 0,
    DGL_ERR_ARG = -1,
    DGL_ERR_ALLOC = -2,
    DGL_ERR_RANGE = -3,
    DGL_ERR_IO = -4,
    DGL_ERR_PARSE = -5
} DGL_Result;

typedef struct DGL_Surface {
    int width;
    int height;
    uint32_t *pixels;
    float *depth;
} DGL_Surface;

typedef struct DGL_Vertex {
    float x;
    float y;
    float z;
    uint32_t color;
} DGL_Vertex;

const char *dgl_version(void);
const char *dgl_result_name(int result);

int dgl_surface_init(DGL_Surface *s, int width, int height);
void dgl_surface_free(DGL_Surface *s);
int dgl_surface_valid(const DGL_Surface *s);
size_t dgl_surface_pixel_count(const DGL_Surface *s);

void dgl_clear(DGL_Surface *s, uint32_t color);
void dgl_clear_depth(DGL_Surface *s, float value);
void dgl_pixel(DGL_Surface *s, int x, int y, uint32_t color);
uint32_t dgl_get_pixel(const DGL_Surface *s, int x, int y);
float dgl_get_depth(const DGL_Surface *s, int x, int y);

void dgl_line(DGL_Surface *s, int x0, int y0, int x1, int y1, uint32_t color);
void dgl_depth_line(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, uint32_t color);
void dgl_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color);
void dgl_fill_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color);
void dgl_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color);
void dgl_fill_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color);
void dgl_wire_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t color);
void dgl_wire_tri_depth(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t color);
void dgl_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c);
void dgl_quad(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, DGL_Vertex d);
void dgl_wire_quad_depth(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, DGL_Vertex d, uint32_t color);
void dgl_char(DGL_Surface *s, int x, int y, uint32_t color, char ch);
void dgl_text(DGL_Surface *s, int x, int y, uint32_t color, const char *text);

uint64_t dgl_hash(const DGL_Surface *s);
int dgl_save_ppm(const DGL_Surface *s, const char *path);
int dgl_parse_color(const char *text, uint32_t *out_color);

#ifdef __cplusplus
}
#endif

#endif
