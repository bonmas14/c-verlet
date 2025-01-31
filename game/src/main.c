// Copyright 2024 bonmas14

// this path is only because i use clangd lsp and cant build with bear
#include "../../raylib-master/src/raylib.h"
#include "../../raylib-master/src/raymath.h"

#include "verlet.h"
#include <stdio.h>

#define TIMESTEP 0.1f
#define POINTS_COUNT 100

Camera3D cam;
simulation_t sim;
Texture2D blank;

void AddNewPoints(void);
void Update(void);
void Draw(void);

int main(void) {
    InitWindow(800, 600, "Verlet");

    SetWindowState(FLAG_VSYNC_HINT /*| FLAG_WINDOW_UNDECORATED*/);

    DisableCursor();

    cam = (Camera3D){ 
        .up = (Vector3) { .y = 1 },
        .fovy = 45.0f,
        .target = 0,
        .position = (Vector3) { .z = -5, .y = 1 },
        .projection = CAMERA_PERSPECTIVE
    };

    sim = (simulation_t) { 0 };

    InitSimulation(&sim, TIMESTEP);

    blank = LoadTexture("resources/blank.png");

    while (!WindowShouldClose()) {
        UpdateCamera(&cam, CAMERA_FIRST_PERSON);

        if (IsKeyDown(KEY_SPACE) && IsKeyDown(KEY_LEFT_SHIFT)) {
            AddNewPoints();
        } else if (IsKeyPressed(KEY_SPACE)) {
            AddNewPoints();
        }

        StepSimulation(&sim, 1);
        Draw();
    }

    MemFree(sim.points);
    CloseWindow();
}

void AddNewPoints(void) {
    verlet_point_t points[POINTS_COUNT] = { 0 };

    for (size_t i = 0; i < POINTS_COUNT; i++) {
        float xo = (float)GetRandomValue(-100, 100) / 10.f;
        float yo = (float)GetRandomValue(-100, 100) / 10.f;
        float zo = (float)GetRandomValue(-100, 100) / 10.f;

        float factor = (float)GetRandomValue(-100, 100) / 10.f;

        Vector2 acc = Vector2Scale(Vector2Normalize((Vector2){ xo, zo }), factor);

        points[i] = (verlet_point_t) {
            .unique_id = (uint32_t)i + 1,
            .scale = 0.1f,
            .current = (Vector3) { .x = 0, .y = 1, .z = 0 },
            .previous = (Vector3) { .x = 0, .y = 1, .z = 0 },
            .acceleration = (Vector3) { .x = acc.x, .y = yo, .z = acc.y },
        };
    }

    AddPoints(&sim, POINTS_COUNT, points);
}

void Draw(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(cam);

    //DrawGrid(10, 1.0f);
    DrawSimulation(&sim);

    EndMode3D();

    DrawFPS(0, 0);

    char count[256];

    sprintf(count, "press <SPACE> to spawn. used:%u alloc:%u (%llu kb)",
            (uint32_t)sim.used_points_count, 
            (uint32_t)sim.allocated_points_count,
            sim.allocated_points_count * sizeof(verlet_point_t) / 1024);

    DrawText(count, 0, 20, 16, WHITE);
    EndDrawing();
}

void DrawSimulation(simulation_t *state) {
    for (size_t i = 0; i < state->used_points_count; i++) {
        verlet_point_t point = state->points[i];

		DrawBillboard(cam, blank, point.current, point.scale, CLITERAL(Color) {255, 255, 255, 128});
    }
}
