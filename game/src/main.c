// Copyright 2024 bonmas14

// this path is only because i use clangd lsp and cant build with bear
#include "../../raylib-master/src/raylib.h"
#include "../../raylib-master/src/raymath.h"

#include "verlet.h"

#define TIMESTEP 0.1f
#define POINTS_COUNT 100

Camera3D cam;
simulation_t sim;

void Draw(void);
void Update(void);

int main(void) {
    InitWindow(800, 600, "Verlet");

    SetWindowState(FLAG_VSYNC_HINT /*| FLAG_WINDOW_UNDECORATED*/);

    cam = (Camera3D){ 
        .up = (Vector3) { .y = 1 },
        .fovy = 45.0f,
        .target = 0,
        .position = (Vector3) { .z = -5, .y = 1 },
        .projection = CAMERA_PERSPECTIVE
    };

    sim = (simulation_t) { .square_timestep = TIMESTEP * TIMESTEP };

    verlet_point_t points[POINTS_COUNT];

    for (size_t i = 0; i < POINTS_COUNT; i++) {
        float xo = (float)GetRandomValue(-100, 100) / 100.f;
        float yo = (float)GetRandomValue(0, 100) / 100.f;
        float zo = (float)GetRandomValue(-100, 100) / 100.f;

        points[i] = (verlet_point_t) {
            .unique_id = 1,
            .scale = 0.1f,
            .current = (Vector3) { .x = xo, .y = 0, .z = zo },
            .previous = (Vector3) { .x = xo, .y = 0, .z = zo },
            .acceleration = (Vector3) { .y = 5 + yo },
        };
    }

    AddPoints(&sim, POINTS_COUNT, points);

    while (!WindowShouldClose()) {

        StepSimulation(&sim, 1);
        Draw();
    }

    MemFree(sim.points);
    CloseWindow();
}

void Draw(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(cam);

    DrawGrid(10, 1.0f);
    DrawSimulation(&sim);

    EndMode3D();

    DrawFPS(0, 0);
    EndDrawing();
}

void DrawSimulation(simulation_t *state) {
    for (size_t i = 0; i < state->used_points_count; i++) {
        verlet_point_t point = state->points[i];

        DrawCubeV(point.current, Vector3Scale(Vector3One(), point.scale), WHITE);
    }
}
