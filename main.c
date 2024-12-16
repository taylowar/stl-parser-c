#include <stdio.h>
#include <assert.h>

#define STALE_IMPLEMENTATION
#define STALE_RAYLIB
#include "stale.h"

#include <raylib.h>

#define FACTOR 100
#define WIDTH  16*FACTOR
#define HEIGHT 10*FACTOR

char* pop_argv(int *argc, char ***argv)
{
    assert(*argc > 0 && "argument stack underflow");
    char *result = **argv;
    (*argc) -= 1;
    (*argv) += 1;
    return result; 
}

void make_camera3d(Vector3 position, Camera3D *c3d)
{
    c3d->position = position;
    c3d->target = (Vector3){0, 0, 0};
    c3d->up = (Vector3){0, 1, 0};
    c3d->fovy = 45;
    c3d->projection = CAMERA_PERSPECTIVE;
}

int main(int argc, char **argv)
{
    char *program = pop_argv(&argc, &argv);
    if (argc == 0) {
        printf("ERROR: missing stl filepath\n");
        printf("usage: %s <filepath.stl>\n", program);
        return 1;
    }
    char *stl_filepath = pop_argv(&argc, &argv);
    StaleArray sa = {0};
    sa_from_file(stl_filepath, &sa);
    FacetArray fa = {0};
    sa_fill_fasects(sa, &fa);

    InitWindow(WIDTH, HEIGHT, "stale");
    SetTargetFPS(60);

    Camera3D c3d = {0};
    make_camera3d((Vector3){80, 0, 80}, &c3d);

    Color c = {18,18,18,255};
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(c);
        UpdateCamera(&c3d, CAMERA_THIRD_PERSON);
        BeginMode3D(c3d);
        DrawGrid(32, 1.f);
        for (size_t i=0;i<fa.count;++i) {
            Facet fct = fa.data[i];
            DrawLine3D(fct.p1, fct.p2, LIME);
            DrawLine3D(fct.p2, fct.p3, SKYBLUE);
            DrawLine3D(fct.p3, fct.p1, RED);
            //DrawTriangle3D(fct.p1, fct.p2, fct.p3, LIME);
        }
        EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
