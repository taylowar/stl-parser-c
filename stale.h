#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define DA_BASE_CAP 256
#define da_append(da, item)                                                          \
    do {                                                                             \
        if ((da)->count == (da)->capacity) {                                         \
            (da)->capacity = (da)->capacity == 0 ? DA_BASE_CAP : (da)->capacity*2;   \
            (da)->data = realloc((da)->data, (da)->capacity*sizeof(*(da)->data));    \
        }                                                                            \
        (da)->data[(da)->count++] = (item);                                          \
    } while (0);

typedef enum {
    SOLID_START = 0,
    SOLID_END   = 1,
    FACET_START = 2,
    FACET_END   = 3,
    LOOP_END    = 4, 
    LOOP_START  = 5,
    VERTEX      = 6,
    UNKNOWN     = 7,
} Keyword ;

typedef struct {
    Keyword kywd;
    char *values;
} Stale;

typedef struct {
    size_t count;
    size_t capacity;
    Stale *data;
} StaleArray;

#ifdef STALE_RAYLIB
#include "raylib.h"
typedef struct {
    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
} Facet;

typedef struct {
    size_t count;
    size_t capacity;
    Facet *data;
} FacetArray;

Vector3 sa_raylib_vec3(Stale item);
void sa_facet_print(Facet fct);
void sa_fill_fasects(StaleArray sa, FacetArray *fa);
#endif

void sa_from_file(const char *filepath, StaleArray *sa);
void sa_print(StaleArray sa);

#ifdef STALE_IMPLEMENTATION
void sa_from_file(const char *filepath, StaleArray *sa)
{
    FILE *fd = fopen(filepath, "r");
    char line[256];
    while (fgets(line, sizeof(line), fd)) {
        // remove the trailing whitespace (left)
        size_t idx = 0;
        while (isspace(line[idx])) {
            idx += 1;
        }
        char new_line[256];
        size_t nlc = 0;
        for (size_t j=idx;j<256;++j) {
            new_line[nlc++] = line[j];
        }
        // chop the first word
        idx = 0;
        while (!isspace(new_line[idx])) {
            idx += 1;
        }
        char kwd_buf[256];
        snprintf(kwd_buf, idx+1, "%s", new_line);
        // chop the rest of the line
        char val_buf[256];
        size_t vsc = 0;
        for (size_t j=idx+1;j<256;++j) {
            if (new_line[j] == '\n') {
                val_buf[vsc++] = 0;
            } else {
                val_buf[vsc++] = new_line[j];
            }
        }
        Keyword kywd = UNKNOWN;
        if (strcmp(kwd_buf, "solid") == 0) {
            kywd = SOLID_START;
        }
        if (strcmp(kwd_buf, "endsolid") == 0) {
            kywd = SOLID_END;
        }
        if (strcmp(kwd_buf, "facet") == 0) {
            kywd = FACET_START;
        }
        if (strcmp(kwd_buf, "outer") == 0) {
            if (strcmp(val_buf, "loop") == 0) {
                kywd = LOOP_START;
            }
        }
        if (strcmp(kwd_buf, "vertex") == 0) {
            kywd = VERTEX;
        }
        if (strcmp(kwd_buf, "endloop") == 0) {
            kywd = LOOP_END;
        }
        if (strcmp(kwd_buf, "endfacet") == 0) {
            kywd = FACET_END;
        }
        Stale item = {0};
        item.kywd = kywd;
        item.values = malloc(vsc);
        snprintf(item.values, vsc, "%s", val_buf);
        da_append(sa, item);
    }
}
void sa_print(StaleArray sa)
{
    for (size_t i=0;i<sa.count;++i) {
        printf("{\n");
        char *kywd_str = "";
        switch (sa.data[i].kywd) {
            case SOLID_START:
                kywd_str = "SOLID_SART";
                break;
            case SOLID_END:
                kywd_str = "SOLID_END";
                break;
            case FACET_START:
                kywd_str = "FACET_START";
                break;
            case FACET_END:
                kywd_str = "FACET_END";
                break;
            case LOOP_START:
                kywd_str = "LOOP_START";
                break;
            case LOOP_END:
                kywd_str = "LOOP_END";
                break;
            case VERTEX:
                kywd_str = "VERTEX";
                break;
            case UNKNOWN:
                kywd_str = "UNKNOWN";
                break;
        }
        printf("    kwd: [%s]\n", kywd_str);
        printf("    value: [%s]\n", sa.data[i].values);
        printf("}\n");
    }
}

#ifdef STALE_RAYLIB
Vector3 sa_raylib_vec3(Stale item)
{
    Vector3 result = {0};
    char *v = item.values;

    char xw[256];
    size_t  xwc = 0;
    while (v[xwc] != ' ') {
        xw[xwc] = v[xwc];
        xwc += 1;
    }
    char xv[256];
    snprintf(xv, xwc, "%s", xw);
    result.x = atof(xv);

    char yw[256];
    size_t ywc = 0;
    for (size_t j=xwc+1;j<256;++j) {
        if (v[j] == ' ') break;
        yw[ywc] = v[j];
        ywc += 1;
    }
    char yv[256];
    snprintf(yv, ywc, "%s", yw);
    result.y = atof(yv);

    char zw[256];
    size_t zwc = 0;
    for (size_t j=xwc+ywc+2;j<256;++j) {
        if (v[j] == '\n') break;
        zw[zwc] = v[j];
        zwc += 1;
    }
    char zv[256];
    snprintf(zv, zwc, "%s", zw);
    result.z = atof(zv);

    return result;
}
void sa_facet_print(Facet fct)
{
    printf("{\n");
    printf("    <%f %f %f>\n", fct.p1.x,fct.p1.y,fct.p1.z);
    printf("    <%f %f %f>\n", fct.p2.x,fct.p2.y,fct.p2.z);
    printf("    <%f %f %f>\n", fct.p3.x,fct.p3.y,fct.p3.z);
    printf("}\n");
}

void sa_fill_fasects(StaleArray sa, FacetArray *fa)
{
    size_t idx=0;
    while (idx < sa.count) {
        if (sa.data[idx].kywd == FACET_START) {
            Facet fct = {0};
            idx+=1;
            if (sa.data[idx].kywd == LOOP_START) {
                idx+=1;
                if (sa.data[idx].kywd == VERTEX) {
                    Vector3 p = sa_raylib_vec3(sa.data[idx]);
                    fct.p1 = p;
                }
                idx+=1;
                if (sa.data[idx].kywd == VERTEX) {
                    Vector3 p = sa_raylib_vec3(sa.data[idx]);
                    fct.p2 = p;
                }
                idx+=1;
                if (sa.data[idx].kywd == VERTEX) {
                    Vector3 p = sa_raylib_vec3(sa.data[idx]);
                    fct.p3 = p;
                }
            }
            da_append(fa, fct);
        }
        idx+=1;
    }
}

#endif

#endif
