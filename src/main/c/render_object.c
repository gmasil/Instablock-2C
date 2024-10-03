#include <GL/glew.h>

#include "calc.h"
#include "render_object.h"
#include "wavefront.h"

struct RenderObject load_render_object(char *file_path) {
    int loaded_faces;
    float *buffer = loadWavefrontObj(file_path, &loaded_faces);
    GLuint bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    // buffer size: 1 face = (3 vertices + 3 normals + 2 text_coords) * 3 floats =
    // 24 floats
    glBufferData(GL_ARRAY_BUFFER, loaded_faces * 24 * sizeof(GLfloat), buffer, GL_STATIC_DRAW);
    struct RenderObject ro = {bufferId, loaded_faces, 0, 0, 0};
    return ro;
}

void unload_render_object(struct RenderObject render_object) {
    glDeleteBuffers(1, &(render_object.id));
}

void render(struct RenderObject render_object, GLuint modelID, float *modelMatrix) {
    // set model location
    set_translation_matrix4(render_object.x, render_object.y, render_object.z, modelMatrix);
    glUniformMatrix4fv(modelID, 1, GL_FALSE, modelMatrix);

    // enable vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, render_object.id);

    // vertices (vector 3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    // normals (vector 3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    // texture coordinates (vector 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

    // render triangles
    glDrawArrays(GL_TRIANGLES, 0, render_object.loaded_faces * 3);

    // disable evrtex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
