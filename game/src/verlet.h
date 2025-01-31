// Copyright 2024 bonmas14
#pragma once

#include "../../raylib-master/src/raylib.h"
#include <stdint.h>

typedef struct {
    uint32_t unique_id;
    float scale;

    Vector3 current;
    Vector3 previous;
    Vector3 acceleration;
} verlet_point_t;

typedef struct {
    float square_timestep;
    size_t used_points_count;
    size_t allocated_points_count;
    verlet_point_t* points;
} simulation_t;


void InitSimulation(simulation_t* sim, float timestep);
// returns false only at critical allocation exception
bool AddPoints(simulation_t* state, size_t count, verlet_point_t* points);

// returns false if it couldnt find one of elements to delete, but deletes all intcould find
bool RemovePoints(simulation_t* state, size_t count, uint32_t* ids);

void StepSimulation(simulation_t* state, size_t steps);

void DrawSimulation(simulation_t* state);
