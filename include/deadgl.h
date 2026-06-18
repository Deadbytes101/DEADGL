#ifndef DEADGL_H
#define DEADGL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DGL_VERSION "0.3.0"
#define DGL_OK 0
#define DGL_ERR -1
#define DGL_FAR 1.0e30f
#define DGL_RGB(r,g,b) (0xff000000u | ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))

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

int dgl_surface_init(DGL_Surface *s, int width, int height);
void dgl_surface_free(DGL_Surface *s);
void dgl_clear(DGL_Surface *s, uint32_t color);
void dgl_clear_depth(DGL_Surface *s, float value);
void dgl_pixel(DGL_Surface *s, int x, int y, uint32_t color);
uint32_t dgl_get_pixel(const DGL_Surface *s, int x, int y);
void dgl_line(DGL_Surface *s, int x0, int y0, int x1, int y1, uint32_t color);
void dgl_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color);
void dgl_fill_rect(DGL_Surface *s, int x, int y, int w, int h, uint32_t color);
void dgl_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color);
void dgl_fill_circle(DGL_Surface *s, int cx, int cy, int r, uint32_t color);
void dgl_wire_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c, uint32_t color);
void dgl_tri(DGL_Surface *s, DGL_Vertex a, DGL_Vertex b, DGL_Vertex c);
uint64_t dgl_hash(const DGL_Surface *s);
int dgl_save_ppm(const DGL_Surface *s, const char *path);

#ifdef __cplusplus
}
#endif

#endif
