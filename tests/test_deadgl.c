#include "deadgl.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int test_surface(void) {
    DGL_Surface s;
    CHECK(dgl_surface_init(&s, 64, 48) == DGL_OK);
    CHECK(s.width == 64);
    CHECK(s.height == 48);
    CHECK(s.pixels != NULL);
    CHECK(s.depth != NULL);
    dgl_surface_free(&s);
    CHECK(s.pixels == NULL);
    return 0;
}

static int test_pixel_line_rect(void) {
    DGL_Surface s;
    CHECK(dgl_surface_init(&s, 32, 32) == DGL_OK);
    dgl_clear(&s, 0);
    dgl_pixel(&s, 1, 1, DGL_RGB(1, 2, 3));
    dgl_pixel(&s, -1, -1, DGL_RGB(9, 9, 9));
    CHECK(dgl_get_pixel(&s, 1, 1) == DGL_RGB(1, 2, 3));
    dgl_line(&s, -5, 0, 20, 0, DGL_RGB(7, 7, 7));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(7, 7, 7));
    dgl_fill_rect(&s, 30, 30, 20, 20, DGL_RGB(4, 4, 4));
    CHECK(dgl_get_pixel(&s, 31, 31) == DGL_RGB(4, 4, 4));
    dgl_surface_free(&s);
    return 0;
}

static int test_circle_tri_hash_ppm(void) {
    DGL_Surface s;
    DGL_Vertex a;
    DGL_Vertex b;
    DGL_Vertex c;
    uint64_t h1;
    uint64_t h2;
    FILE *f;
    char magic[3] = {0, 0, 0};
    CHECK(dgl_surface_init(&s, 64, 64) == DGL_OK);
    dgl_clear(&s, 0);
    dgl_fill_circle(&s, 32, 32, 8, DGL_RGB(9, 8, 7));
    CHECK(dgl_get_pixel(&s, 32, 32) == DGL_RGB(9, 8, 7));
    a = (DGL_Vertex){4.0f, 4.0f, 0.9f, DGL_RGB(200, 0, 0)};
    b = (DGL_Vertex){60.0f, 4.0f, 0.9f, DGL_RGB(200, 0, 0)};
    c = (DGL_Vertex){32.0f, 60.0f, 0.9f, DGL_RGB(200, 0, 0)};
    dgl_clear_depth(&s, DGL_FAR);
    dgl_tri(&s, a, b, c);
    h1 = dgl_hash(&s);
    h2 = dgl_hash(&s);
    CHECK(h1 != 0u);
    CHECK(h1 == h2);
    a.x = NAN;
    dgl_tri(&s, a, b, c);
    CHECK(dgl_save_ppm(&s, "deadgl_test.ppm") == DGL_OK);
    f = fopen("deadgl_test.ppm", "rb");
    CHECK(f != NULL);
    CHECK(fread(magic, 1, 2, f) == 2);
    fclose(f);
    remove("deadgl_test.ppm");
    CHECK(magic[0] == 'P' && magic[1] == '6');
    dgl_surface_free(&s);
    return 0;
}

int main(void) {
    CHECK(test_surface() == 0);
    CHECK(test_pixel_line_rect() == 0);
    CHECK(test_circle_tri_hash_ppm() == 0);
    puts("PASS deadgl core");
    return 0;
}
