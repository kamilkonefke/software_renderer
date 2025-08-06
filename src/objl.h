#ifndef _OBJL_H_
#define _OBJL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void load_obj(const char* path, double** vertex_data_out, int* vertex_size_out, int** index_data_out, int* index_size_out) {
    FILE* file = fopen(path, "r");

    double* vertex_data = NULL;
    int vertex_size = 0;

    int* index_data = NULL;
    int index_size = 0;

    char line[128];
    for (;;) {
        int res = fscanf(file, "%s", line);
        if (res == EOF) {
            break;
        }

        // vertex data
        if (strcmp(line, "v") == 0) {
            double x, y, z;
            fscanf(file, "%lf %lf %lf", &x, &y , &z);

            vertex_data = (double*)realloc(vertex_data, sizeof(double) * (vertex_size + 3));

            vertex_data[vertex_size] = x;
            vertex_data[vertex_size + 1] = y;
            vertex_data[vertex_size + 2] = z;

            vertex_size += 3;
        }
        else if (strcmp(line, "f") == 0) { // Indexes
            int i1, i2, i3;
            int t1, t2, t3;
            int n1, n2, n3;
            fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &i1, &t1, &n1, &i2, &t2, &n2, &i3, &t3, &n3);

            index_data = (int*)realloc(index_data, sizeof(int) * (index_size + 3));

            index_data[index_size] = i1 - 1;
            index_data[index_size + 1] = i2 - 1;
            index_data[index_size + 2] = i3 - 1;

            index_size += 3;
        }
    }
    fclose(file);

    *vertex_data_out = vertex_data;
    *vertex_size_out = vertex_size;
    *index_data_out = index_data;
    *index_size_out = index_size;
}

static void free_obj(double* vertex_data, int* index_data) {
    free(vertex_data);
    free(index_data);
}

#endif
