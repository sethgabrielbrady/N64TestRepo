#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3ddebug.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

//https://www.opengl.org/archives/resources/code/samples/glut_examples/examples/examples.html


static float light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
static float light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
static float n[6][3] = {  /* Normals for the 6 faces of a cube. */
    {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}
};
static int faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
    {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
    {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3}
};
static float v[8][3];  /* Will be filled in with X,Y,Z vertexes. */

void drawBox(void) {
    int i;

    for (i = 0; i < 6; i++) {
        t3d_begin(T3D_QUADS);
        t3d_normal3f(n[i][0], n[i][1], n[i][2]);
        t3d_vertex3f(v[faces[i][0]][0], v[faces[i][0]][1], v[faces[i][0]][2]);
        t3d_vertex3f(v[faces[i][1]][0], v[faces[i][1]][1], v[faces[i][1]][2]);
        t3d_vertex3f(v[faces[i][2]][0], v[faces[i][2]][1], v[faces[i][2]][2]);
        t3d_vertex3f(v[faces[i][3]][0], v[faces[i][3]][1], v[faces[i][3]][2]);
        t3d_end();
    }
}

void init(void) {
    /* Setup cube vertex data. */
    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

    /* Initialize Tiny3D */
    t3d_init();
    t3d_light_enable(0, true);
    t3d_light_set_diffuse(0, light_diffuse);
    t3d_light_set_position(0, light_position);

    /* Setup the view of the cube. */
    t3d_projection_matrix(T3D_PERSPECTIVE, 40.0f, 1.0f, 1.0f, 10.0f);
    t3d_look_at(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    /* Adjust cube position to be aesthetic angle. */
    t3d_translatef(0.0f, 0.0f, -1.0f);
    t3d_rotatef(60.0f, 1.0f, 0.0f, 0.0f);
    t3d_rotatef(-20.0f, 0.0f, 0.0f, 1.0f);
}

void display(void) {
    display_context_t disp = 0;

    while (!(disp = display_lock()));
    graphics_fill_screen(disp, graphics_make_color(0, 0, 0, 255));

    t3d_attach_display(disp);
    t3d_clear_color(0, 0, 0, 255);
    t3d_clear_depth();

    drawBox();

    t3d_detach_display();
    display_show(disp);
}

int main(void) {
    /* Initialize libdragon */
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    /* Initialize the cube and Tiny3D */
    init();

    /* Main loop */
    while (1) {
        display();
    }

    return 0;
}