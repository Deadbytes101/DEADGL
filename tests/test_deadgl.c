#include "deadgl.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

int main(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    uint32_t c = 0;
    uint64_t before;
    uint64_t after;
    DGL_Vertex fa;
    DGL_Vertex fb;
    DGL_Vertex fc;
    DGL_Vertex na;
    DGL_Vertex nb;
    DGL_Vertex nc;
    CHECK(strcmp(dgl_version(), "11.0.1") == 0);
    CHECK(dgl_surface_init(&s, 64, 48) == DGL_OK);
    CHECK(dgl_surface_valid(&s));
    CHECK(dgl_surface_pixel_count(&s) == 64u * 48u);
    CHECK(dgl_parse_color("#112233", &c) == DGL_OK);
    CHECK(c == DGL_RGB(0x11, 0x22, 0x33));
    dgl_clear(&s, DGL_RGB(1, 2, 3));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(1, 2, 3));
    dgl_line(&s, 0, 0, 63, 47, DGL_RGB(7, 7, 7));
    before = dgl_hash(&s);
    dgl_text(&s, 2, 2, DGL_RGB(255, 255, 255), "DGL");
    after = dgl_hash(&s);
    CHECK(after != before);
    before = after;
    dgl_tile_grid(&s, 8, DGL_RGB(8, 10, 16), DGL_RGB(18, 22, 32));
    dgl_tile_frame(&s, 8, DGL_RGB(0, 255, 153));
    after = dgl_hash(&s);
    CHECK(after != before);
    dgl_clear(&s, DGL_RGB(0, 0, 0));
    dgl_line(&s, -1000000, 12, 1000000, 12, DGL_RGB(9, 9, 9));
    CHECK(dgl_get_pixel(&s, 0, 12) == DGL_RGB(9, 9, 9));
    CHECK(dgl_get_pixel(&s, 63, 12) == DGL_RGB(9, 9, 9));
    dgl_fill_rect(&s, -8, -8, 16, 16, DGL_RGB(4, 5, 6));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(4, 5, 6));
    CHECK(dgl_get_pixel(&s, 7, 7) == DGL_RGB(4, 5, 6));
    dgl_clear(&s, DGL_RGB(0, 0, 0));
    dgl_clear_depth(&s, DGL_FAR);
    fa.x = 8.0f; fa.y = 8.0f; fa.z = 9.0f; fa.color = DGL_RGB(160, 0, 0);
    fb.x = 56.0f; fb.y = 8.0f; fb.z = 9.0f; fb.color = DGL_RGB(160, 0, 0);
    fc.x = 32.0f; fc.y = 40.0f; fc.z = 9.0f; fc.color = DGL_RGB(160, 0, 0);
    na.x = 8.0f; na.y = 8.0f; na.z = 3.0f; na.color = DGL_RGB(0, 160, 0);
    nb.x = 56.0f; nb.y = 8.0f; nb.z = 3.0f; nb.color = DGL_RGB(0, 160, 0);
    nc.x = 32.0f; nc.y = 40.0f; nc.z = 3.0f; nc.color = DGL_RGB(0, 160, 0);
    dgl_tri(&s, fa, fb, fc);
    dgl_tri(&s, na, nb, nc);
    CHECK(dgl_get_pixel(&s, 32, 20) == DGL_RGB(0, 160, 0));
    CHECK(dgl_get_depth(&s, 32, 20) < 4.0f);
    CHECK(dgl_save_ppm(&s, "deadgl_test.ppm") == DGL_OK);
    remove("deadgl_test.ppm");
    dgl_surface_free(&s);
    puts("PASS deadgl core");
    return 0;
}
