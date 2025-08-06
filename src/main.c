#include <math.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#include "objl.h"

#define FOCAL_LENGTH 2
#define SCALE 30

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* surface;

double rot = 0.2;

Point perspective(Vertex vertex) {
    Point this = {};
    this.x = 64 + (FOCAL_LENGTH * vertex.x) / (FOCAL_LENGTH + vertex.z) * SCALE;
    this.y = 64 + (FOCAL_LENGTH * vertex.y) / (FOCAL_LENGTH + vertex.z) * SCALE;
    return this;
}

Vertex rotate_x(Vertex vertex, double radians) {
    Vertex this = {};
    this.x = vertex.x;
    this.y = vertex.y * cos(radians) - vertex.z * sin(radians);
    this.z = vertex.y * sin(radians) + vertex.z * cos(radians);
    return this;
}

Vertex rotate_y(Vertex vertex, double radians) {
    Vertex this = {};
    this.x = vertex.x * cos(radians) - vertex.z * sin(radians);
    this.y = vertex.y;
    this.z = vertex.x * sin(radians) + vertex.z * cos(radians);
    return this;
}

Vertex rotate_z(Vertex vertex, double radians) {
    Vertex this = {};
    this.x = vertex.x * cos(radians) + vertex.y * sin(radians);
    this.y = vertex.y * cos(radians) - vertex.x * sin(radians);
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
    window = SDL_CreateWindow("software renderer", 512, 512, SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS);
    renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderScale(renderer, 4.0, 4.0);

    if (argc == 1) {
        printf("No .obj file picked.");
        return 1;
    }

    double* vertex_buffer = NULL;
    int vertex_buffer_size = 0;

    int* index_buffer = NULL; 
    int index_buffer_size = 0;

    load_obj(argv[1], &vertex_buffer, &vertex_buffer_size, &index_buffer, &index_buffer_size);

    bool is_running = true;
    SDL_Event event;
    while (is_running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < vertex_buffer_size; i += 3) {
            Vertex rv = {vertex_buffer[i], vertex_buffer[i+1], vertex_buffer[i+2]};
            rv = rotate_y(rv, rot);
            vertex_buffer[i] = rv.x;
            vertex_buffer[i+1] = rv.y;
            vertex_buffer[i+2] = rv.z;
        }

        draw_buffer_indexed(vertex_buffer, vertex_buffer_size, index_buffer, index_buffer_size);
        SDL_RenderPresent(renderer);

        SDL_Delay(50);
    }

    free(vertex_buffer);
    free(index_buffer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
