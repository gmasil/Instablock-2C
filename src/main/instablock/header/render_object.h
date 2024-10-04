#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <GLFW/glfw3.h>

struct RenderObject {
    GLuint id;
    int loaded_faces;
    float x, y, z;
};

struct RenderObject load_render_object(char *file_path);
void unload_render_object(struct RenderObject render_object);
void render(struct RenderObject render_object, GLuint modelID, float *modelMatrix);

#endif // RENDER_OBJECT_H
