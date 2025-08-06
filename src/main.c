#include <math.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#include "objl.h"

#define FOV 1
#define SCALE 35

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;

double rot = 0.0;

typedef struct Vec3 {
    double x;
    double y;
    double z;
} Vec3, Vertex;

typedef struct Vec2 {
    double x;
    double y;
} Vec2, Point;

Vec2 perspective(Vertex vertex) {
    Vec2 this = {};
    this.x = 64 + (FOV * vertex.x) / (FOV + vertex.z) * SCALE;
    this.y = 64 + (FOV * vertex.y) / (FOV + vertex.z) * SCALE;
    return this;
}

Vec3 rotate_x(Vec3 vertex, double rot) {
    Vec3 this = {};
    this.x = vertex.x;
    this.y = vertex.y * cos(rot) - vertex.z * sin(rot);
    this.z = vertex.y * sin(rot) + vertex.z * cos(rot);
    return this;
}

Vec3 rotate_y(Vec3 vertex, double rot) {
    Vec3 this = {};
    this.x = vertex.x * cos(rot) - vertex.z * sin(rot);
    this.y = vertex.y;
    this.z = vertex.x * sin(rot) + vertex.z * cos(rot);
    return this;
}

Vec3 rotate_z(Vec3 vertex, double rot) {
    Vec3 this = {};
    this.x = vertex.x * cos(rot) + vertex.y * sin(rot);
    this.y = vertex.y * cos(rot) - vertex.x * sin(rot);
    this.z = vertex.z;
    return this;
}

void draw_buffer_indexed(double buffer[], size_t buffer_size, int indicies[], size_t indicies_size) {
    for (size_t i = 0; i < indicies_size; i += 3) {

        int i1 = indicies[i] * 3;
        int i2 = indicies[i+1] * 3;
        int i3 = indicies[i+2] * 3;

        Vertex b1 = { buffer[i1], buffer[i1 + 1], buffer[i1 + 2] };
        Vertex b2 = { buffer[i2], buffer[i2 + 1], buffer[i2 + 2] };
        Vertex b3 = { buffer[i3], buffer[i3 + 1], buffer[i3 + 2] };

        Point v1 = perspective(b1);
        Point v2 = perspective(b2);
        Point v3 = perspective(b3);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderLine(renderer, v1.x, v1.y, v2.x, v2.y);
        SDL_RenderLine(renderer, v2.x, v2.y, v3.x, v3.y);
        SDL_RenderLine(renderer, v3.x, v3.y, v1.x, v1.y);
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("renderer", 512, 512, SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS);
    renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderScale(renderer, 4.0, 4.0);

    if (argc == 1) {
        printf("No .obj file.");
        return 1;
    }

    double* vertex_data = NULL;
    int vertex_size = 0;

    int* index_data = NULL;
    int index_size = 0;

    load_obj(argv[1], &vertex_data, &vertex_size, &index_data, &index_size);

    bool is_running = true;

    SDL_Event event;
    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        rot += 0.02;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < vertex_size; i += 3) {
            Vertex rv = {vertex_data[i], vertex_data[i+1], vertex_data[i+2]};
            rv = rotate_y(rv, rot);
            vertex_data[i] = rv.x;
            vertex_data[i+1] = rv.y;
            vertex_data[i+2] = rv.z;
        }

        draw_buffer_indexed(vertex_data, vertex_size, index_data, index_size);
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    free(vertex_data);
    free(index_data);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
