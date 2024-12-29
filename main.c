#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <assert.h>

#define STALE_IMPLEMENTATION
#define STALE_RAYLIB
#include "stale.h"

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

typedef struct {
    Vector3 pos;
    Vector3 size;
    Color tint;
} Cursor;

typedef struct {
    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
    Color tint;
} Cube;

typedef struct {
    size_t count;
    size_t capacity;
    Cube *data;
} Cuba;

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
    SetTargetFPS(40);

    Camera3D c3d = {0};
    make_camera3d((Vector3){30, 0, 30}, &c3d);

    Cursor cur = {0};
    cur.size = (Vector3){4, 1, 4};
    cur.pos = (Vector3){-16+cur.size.x/2,cur.size.y/2,-16+cur.size.z/2};
    cur.tint = YELLOW;

    int xdir = 1;
    int zdir = 1;

    size_t rndr = 1;

    Cuba ca = {0};

    Color c = {18,18,18,255};
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_I)) {
            rndr = (rndr+1)%3;
        }
        if (xdir == 1) {
            if (zdir == 1) {
                if (cur.pos.z < 16-cur.size.z/2) {
                    cur.pos.z += cur.size.z;
                } else {
                    cur.pos.x += cur.size.x;
                    zdir = -1;
                }
            } else if (zdir == -1) {
                if (cur.pos.z > -16+cur.size.z/2) {
                    cur.pos.z -= cur.size.z;
                } else {
                    cur.pos.x += cur.size.x;
                    zdir = 1;
                }
            }
            if (cur.pos.x > 16-cur.size.x/2) {
                xdir = -1;
                cur.pos.y += cur.size.y;
                cur.tint = cur.tint.r == YELLOW.r && cur.tint.g == YELLOW.g ? VIOLET : YELLOW;
            }
        } else if (xdir == -1) {
            if (zdir == 1) {
                if (cur.pos.z < 16-cur.size.z/2) {
                    cur.pos.z += cur.size.z;
                } else {
                    cur.pos.x -= cur.size.x;
                    zdir = -1;
                }
            } else if (zdir == -1) {
                if (cur.pos.z > -16+cur.size.z/2) {
                    cur.pos.z -= cur.size.z;
                } else {
                    cur.pos.x -= cur.size.x;
                    zdir = 1;
                }
            }
            if (cur.pos.x < -16+cur.size.x/2) {
                xdir = 1;
                cur.pos.y += cur.size.y;
                cur.tint = cur.tint.r == YELLOW.r && cur.tint.g == YELLOW.g ? VIOLET : YELLOW;
            }
        }
        ca.count = 0;
        float y = cur.pos.y;
        for (size_t i=0;i<fa.count;++i) {
            Facet fct = fa.data[i];
            Vector3 p1 = fct.p1;
            Vector3 p2 = fct.p2;
            Vector3 p3 = fct.p3;
            if (p1.y < p2.y) {
                if (p2.y < p3.y) {
                    if (p3.y > y) {
                        if (p2.y == p3.y) {
                            p2.y = y;
                        }
                        p3.y = y;
                    }
                } else {
                    if (p2.y > y) {
                        if (p2.y == p3.y) {
                            p3.y = y;
                        }
                        p2.y = y;
                    }
                }
            } else {
                if (p1.y > y) {
                    if (p1.y == p2.y) {
                        p2.y = y;
                    }
                    p1.y = y;
                }
            }

            if (p1.y <= y && p2.y <= y && p3.y <= y) {
                Cube c = {0};
                c.p1 = p1;
                c.p2 = p2;
                c.p3 = p3;
                c.tint = RAYWHITE;
                da_append(&ca, c);
            }
        }
        BeginDrawing();
        ClearBackground(c);
        UpdateCamera(&c3d, CAMERA_THIRD_PERSON);
        BeginMode3D(c3d);
        DrawGrid(32, 1.f);
        DrawCube(cur.pos, cur.size.x, cur.size.y, cur.size.z, cur.tint);
        if (rndr) {
            for (size_t i=0;i<ca.count;++i) {
                Cube cube = ca.data[i];
                DrawTriangle3D(cube.p3, cube.p2, cube.p1, cube.tint);
            }
        }
        EndMode3D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
