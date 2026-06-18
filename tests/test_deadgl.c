#include "deadgl.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int test_surface_contract(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    CHECK(strcmp(dgl_version(), "1.1.0") == 0);
    CHECK(dgl_surface_init(NULL, 1, 1) == DGL_ERR_ARG);
    CHECK(dgl_surface_init(&s, 0, 1) == DGL_ERR_ARG);
    CHECK(dgl_surface_init(&s, DGL_MAX_DIM + 1, 1) == DGL_ERR_ARG);
    CHECK(dgl_surface_init(&s, 64, 48) == DGL_OK);
    CHECK(dgl_surface_valid(&s));
    CHECK(dgl_surface_pixel_count(&s) == 64u * 48u);
    dgl_surface_free(&s);
    CHECK(!dgl_surface_valid(&s));
    return 0;
}

static int test_color_parser(void) {
    uint32_t c = 0;
    CHECK(dgl_parse_color("#112233", &c) == DGL_OK);
    CHECK(c == DGL_RGB(0x11, 0x22, 0x33));
    CHECK(dgl_parse_color("0x80112233", &c) == DGL_OK);
    CHECK(c == 0x80112233u);
    CHECK(dgl_parse_color("xyz", &c) == DGL_ERR_PARSE);
    CHECK(dgl_parse_color("#ffffffffff", &c) == DGL_ERR_PARSE);
    return 0;
}

static int test_primitives_clip(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    CHECK(dgl_surface_init(&s, 32, 32) == DGL_OK);
    dgl_clear(&s, 0u);
    dgl_pixel(&s, 1, 1, DGL_RGB(1, 2, 3));
    dgl_pixel(&s, -1, -1, DGL_RGB(9, 9, 9));
    CHECK(dgl_get_pixel(&s, 1, 1) == DGL_RGB(1, 2, 3));
    dgl_line(&s, -1000, 0, 1000, 0, DGL_RGB(7, 7, 7));
    CHECK(dgl_get_pixel(&s, 0, 0) == DGL_RGB(7, 7, 7));
    CHECK(dgl_get_pixel(&s, 31, 0) == DGL_RGB(7, 7, 7));
    dgl_fill_rect(&s, 30, 30, 1000, 1000, DGL_RGB(4, 4, 4));
    CHECK(dgl_get_pixel(&s, 31, 31) == DGL_RGB(4, 4, 4));
    dgl_rect(&s, 2, 2, 6, 6, DGL_RGB(8, 8, 8));
    CHECK(dgl_get_pixel(&s, 2, 2) == DGL_RGB(8, 8, 8));
    dgl_surface_free(&s);
    return 0;
}

static int test_circle_tri_hash_ppm(void) {
    DGL_Surface s = {0, 0, NULL, NULL};
    DGL_Vertex a;
    DGL_Vertex b;
    DGL_Vertex c;
    uint64_t h1;
    uint64_t h2;
    FILE *f;
    char magic[3] = {0, 0, 0};

    CHECK(dgl_surface_init(&s, 64, 64) == DGL_OK);
    dgl_clear(&s, 0u);
    dgl_fill_circle(&s, 32, 32, 8, DGL_RGB(9, 8, 7));
    CHECK(dgl_get_pixel(&s, 32, 32) == DGL_RGB(9, 8, 7));

    a = (DGL_Vertex){4.0f, 4.0f, 0.9f, DGL_RGB(200, 0, 0)};
    b = (DGL_Vertex){60.0f, 4.0f, 0.9f, DGL_RGB(0, 200, 0)};
    c = (DGL_Vertex){32.0f, 60.0f, 0.9f, DGL_RGB(0, 0, 200)};
    dgl_clear_depth(&s, DGL_FAR);
    dgl_tri(&s, a, b, c);
    h1 = dgl_hash(&s);
    h2 = dgl_hash(&s);
    CHECK(h1 != 0u);
    CHECK(h1 == h2);
    CHECK(dgl_get_depth(&s, 32, 20) < DGL_FAR);

    a.x = NAN;
    dgl_tri(&s, a, b, c);
    CHECK(dgl_save_ppm(&s, "deadgl_test.ppm") == DGL_OK);
    f = fopen("deadgl_test.ppm", "rb");
    CHECK(f != NULL);
    CHECK(fread(magic, 1u, 2u, f) == 2u);
    fclose(f);
    remove("deadgl_test.ppm");
    CHECK(magic[0] == 'P' && magic[1] == '6');
    dgl_surface_free(&s);
    return 0;
}

int main(void) {
    CHECK(test_surface_contract() == 0);
    CHECK(test_color_parser() == 0);
    CHECK(test_primitives_clip() == 0);
    CHECK(test_circle_tri_hash_ppm() == 0);
    puts("PASS deadgl core");
    return 0;
}
