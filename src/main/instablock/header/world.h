#ifndef WORLD_H
#define WORLD_H

#include "render_object.h"

struct World {
    struct RenderObject *cubes;
    int cube_amount;
};

struct World generate_world(int world_size);
void destroy_world(struct World world);
int is_block_at(struct World world, int x, int y, int z);
void render_world(struct World world, GLuint modelID, GLuint textureID);

#endif // WORLD_H
