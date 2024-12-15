#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

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

#define DA_BASE_CAP 256
#define da_append(da, item)                                                          \
    do {                                                                             \
        if ((da)->count == (da)->capacity) {                                         \
            (da)->capacity = (da)->capacity == 0 ? DA_BASE_CAP : (da)->capacity*2;   \
            (da)->data = realloc((da)->data, (da)->capacity*sizeof(*(da)->data));    \
        }                                                                            \
        (da)->data[(da)->count++] = (item);                                          \
    } while (0);

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

char* pop_argv(int *argc, char ***argv)
{
    assert(*argc > 0 && "argument stack underflow");
    char *result = **argv;
    (*argc) -= 1;
    (*argv) += 1;
    return result; 
}

// 0th -> program name
int main(int argc, char **argv)
{
    char *program_name = pop_argv(&argc, &argv);
    if (argc == 0) {
        printf("[ERROR] missing STL filepath\n");
        printf("usage: %s <filepath.stl>\n", program_name);
        return 1;
    }
    char *stl_filepath = pop_argv(&argc, &argv);
    StaleArray sa = {0};
    sa_from_file(stl_filepath, &sa);
    sa_print(sa);
    return 0;
}
