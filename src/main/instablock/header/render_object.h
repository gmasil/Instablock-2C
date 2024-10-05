#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include <GLFW/glfw3.h>

struct RenderObject {
    GLuint id;
    int loaded_faces;
    float x, y, z;
    float *modelMatrix;
    int texture;
};

struct RenderObject load_render_object(char *file_path, int texture);
void unload_render_object(struct RenderObject render_object);
void render(struct RenderObject render_object, GLuint modelID, GLuint textureID);
void set_position(struct RenderObject render_object, float x, float y, float z);

#endif // RENDER_OBJECT_H
