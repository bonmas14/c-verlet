// Copyright 2024 bonmas14

#include <stdbool.h>
#include <stdint.h>
#include "verlet.h"
#include "../../raylib-master/src/raymath.h"


bool AddPoints(simulation_t *state, size_t count, verlet_point_t *points) {
    size_t available = state->allocated_points_count - state->used_points_count;
    
    if (available > state->allocated_points_count)
        TraceLog(LOG_ERROR, "Invalid data in AddPoints. Allocated:%u Used:%u",
                state->allocated_points_count, state->used_points_count);
    
    if (available < count) {
        TraceLog(LOG_INFO, "Allocating new memory required.");

        size_t to_allocate = (state->used_points_count + count * count);

        to_allocate = to_allocate > 10 ? to_allocate : 10;

        verlet_point_t* new_array = MemRealloc(state->points, to_allocate * sizeof(verlet_point_t));

        if (new_array == NULL) {
            TraceLog(LOG_FATAL, "Error reallocating memory for points");
            return false;
        }

        state->allocated_points_count = to_allocate;
        state->points = new_array;
    } else {
        TraceLog(LOG_INFO, "Allocating new memory is not required. Using available space");
    }

    for (size_t i = 0; i < count; i++) {
        state->points[i + state->used_points_count] = points[i];
    }

    state->used_points_count += count;

    return true;
} 

void SwapElements(size_t from, size_t to, verlet_point_t* array) {
    verlet_point_t temp = array[to];
    array[to] = array[from];
    array[from] = temp;
}

bool RemovePoints(simulation_t *state, size_t count, uint32_t *ids) {
    size_t found_count = 0;

    bool status = true;

    for (size_t i = found_count; i < count; i++) {
        bool found = false;

        for (size_t j = 0; j < state->used_points_count; j++) {
            if (state->points[j].unique_id == ids[i]) {
                found = true;

                state->points[j].unique_id = 0;

                SwapElements(i, state->used_points_count, state->points);

                state->used_points_count--;
                found_count++;
                break;
            }
        }

        if (!found) {
            TraceLog(LOG_ERROR, "couldn't find element from array to delete, skipping");
            status = false;
        }
    }
    return status;
}

void StepSimulation(simulation_t *state, size_t steps) {
    Vector3 gravity = Vector3Scale((Vector3) { .y = -9.81f }, state->square_timestep);

    for (size_t step = 0; step < steps; step++) {
        // collision with floor
        for (size_t i = 0; i < state->used_points_count; i++) {
            verlet_point_t *point = &(state->points[i]);

            if (point->current.y < 0) {
                float disp = point->current.y - point->previous.y;

                point->current.y = 0;
                point->previous.y = disp * 0.40f;
            }
        }

        for (size_t i = 0; i < state->used_points_count; i++) {
            verlet_point_t *point = &(state->points[i]);
            

            Vector3 disp = Vector3Subtract(point->current, point->previous);
            point->previous = point->current;
            point->current = Vector3Add(point->current, disp);

            point->acceleration = Vector3Add(point->acceleration, gravity);
            point->acceleration = Vector3Scale(point->acceleration, state->square_timestep);
            point->current = Vector3Add(point->current, point->acceleration);

        }
    }
}
