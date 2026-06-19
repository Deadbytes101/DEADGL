#include "deadgl.h"

static int clamp_hi(int v, int hi) {
    return v < hi ? v : hi;
}

void dgl_tile_grid(DGL_Surface *s, int tile, uint32_t a, uint32_t b) {
    int ty;
    if (!s || !s->pixels || s->width <= 0 || s->height <= 0 || tile <= 0) { return; }
    for (ty = 0; ty < s->height; ty += tile) {
        int tx;
        int y1 = clamp_hi(ty + tile, s->height);
        for (tx = 0; tx < s->width; tx += tile) {
            int x1 = clamp_hi(tx + tile, s->width);
            int use_a = (((tx / tile) + (ty / tile)) & 1) == 0;
            int y;
            for (y = ty; y < y1; y++) {
                int x;
                for (x = tx; x < x1; x++) {
                    dgl_pixel(s, x, y, use_a ? a : b);
                }
            }
        }
    }
}

void dgl_tile_frame(DGL_Surface *s, int tile, uint32_t color) {
    int ty;
    if (!s || !s->pixels || s->width <= 0 || s->height <= 0 || tile <= 0) { return; }
    for (ty = 0; ty < s->height; ty += tile) {
        int tx;
        for (tx = 0; tx < s->width; tx += tile) {
            int w = clamp_hi(tile, s->width - tx);
            int h = clamp_hi(tile, s->height - ty);
            dgl_rect(s, tx, ty, w, h, color);
        }
    }
}
