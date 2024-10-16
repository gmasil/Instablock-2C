#include <stdlib.h>

#include "world.h"

void add_cube(struct RenderObject *cubes, int *current_index, int x, int y, int z) {
    cubes[*current_index] = load_render_object("cube.obj", 0);
    set_position(&(cubes[*current_index]), x, y, z);
    (*current_index)++;
}

struct World generate_world(int world_size) {
    // TODO: simplex noise
    int current_index          = 0;
    int cube_amount            = world_size * world_size + 3;
    struct RenderObject *cubes = malloc(cube_amount * sizeof(struct RenderObject));
    for (int x = 0; x < world_size; x++) {
        for (int y = 0; y < world_size; y++) {
            add_cube(cubes, &current_index, (world_size / 2) - x, -5, (world_size / 2) + y);
        }
    }
    add_cube(cubes, &current_index, 0, -4, world_size + 0);
    add_cube(cubes, &current_index, 1, -4, world_size + 0);
    add_cube(cubes, &current_index, 0, -4, world_size + 1);
    struct World world = {cubes, cube_amount};
    return world;
}

void destroy_world(struct World world) {
    for (int i = 0; i < world.cube_amount; i++) {
        unload_render_object(world.cubes[i]);
    }
    free(world.cubes);
}

int is_block_at(struct World world, int x, int y, int z) {
    // TODO: performance
    for (int i = 0; i < world.cube_amount; i++) {
        if (world.cubes[i].x == x && world.cubes[i].y == y && world.cubes[i].z == z) {
            return 1;
        }
    }
    return 0;
}

void render_world(struct World world, GLuint modelID, GLuint textureID) {
    for (int i = 0; i < world.cube_amount; i++) {
        render(world.cubes[i], modelID, textureID);
    }
}

int get_ground_under_player(struct World world, float x, float y, float z) {
    int bx = x;
    int by = y;
    int bz = z;
    while (!is_block_at(world, bx, by, bz) && by > -10) {
        by--;
    }
    return by;
}
