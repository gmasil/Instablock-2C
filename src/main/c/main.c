#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "bmp.h"
#include "calc.h"
#include "load_shader.h"
#include "render_object.h"
#include "wavefront.h"

#define PI_HALF 1.5707964f

// translation + rotation -> view matrix exists, name unknown

/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int
mods) { if (key == GLFW_KEY_W && action == GLFW_PRESS) { camera_pos[2] -= 1.0f;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        camera_pos[2] += 1.0f;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        camera_pos[0] -= 1.0f;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        camera_pos[0] += 1.0f;
    }
}
*/

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    GLFWwindow *window; // (In the accompanying source code, this variable is
                        // global for simplicity)
    window = glfwCreateWindow(1024, 768, "Tutorial 01", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are "
                        "not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    GLuint programID    = load_shaders("src/main/shader/simple.vert", "src/main/shader/simple.frag");
    GLuint modelID      = glGetUniformLocation(programID, "model");
    GLuint viewID       = glGetUniformLocation(programID, "view");
    GLuint projectionID = glGetUniformLocation(programID, "projection");
    GLuint textureID    = glGetUniformLocation(programID, "tex_sampler");
    GLuint cameraID     = glGetUniformLocation(programID, "camera_pos");

    // projection for clip space
    float *projectionMatrix = perspective(to_radians(45), 1024.0f / 768.0f, 0.001f, 10000.0f);

    // view matrix (move world away from camera)
    float *viewMatrix = create_matrix4();
    float *camera_pos = create_vector3(0.0f, 1.0f, 20.0f);
    float *camera_dir = create_vector3(0.0f, 0.0f, -1.0f);
    // new camera stuff
    float yaw        = 0;
    float pitch      = 0;
    float rot_speed  = 0.0005f;
    float walk_speed = 10.0f;

    // position of loaded object
    float *modelMatrix = create_matrix4();

    struct RenderObject obj1 = load_render_object("demo.obj");
    struct RenderObject obj2 = load_render_object("demo2.obj");
    obj2.z                   = -5;

    // load textures
    GLuint demo_texture  = load_bmp("demo.bmp");
    GLuint demo_texture2 = load_bmp("demo2.bmp");

    // glfwSetKeyCallback(window, key_callback);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // enable backface culling
    glEnable(GL_CULL_FACE);

    // time
    float current_time = glfwGetTime();
    float last_time    = current_time;
    float delta_time;
    // fps
    long frames_rendered_total = 0;
    float last_fps_eval        = current_time;
    // allow unlimited FPS for testing
    // glfwSwapInterval(0);

    // mouse
    double mouse_x, mouse_y;
    double mouse_last_x, mouse_last_y;
    double mouse_delta_x, mouse_delta_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    do {
        last_time    = current_time;
        current_time = glfwGetTime();
        delta_time   = current_time - last_time;

        mouse_last_x = mouse_x;
        mouse_last_y = mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        mouse_delta_x = mouse_x - mouse_last_x;
        mouse_delta_y = mouse_y - mouse_last_y;

        frames_rendered_total++;
        if (current_time - last_fps_eval >= 1.0) {
            printf("FPS: %ld, Frametime: %f ms/frame\n", frames_rendered_total, 1000.0 / (double)frames_rendered_total);
            frames_rendered_total = 0;
            last_fps_eval         = current_time;
        }

        // calculate camera direction
        yaw += mouse_delta_x * rot_speed;
        pitch += mouse_delta_y * rot_speed;
        // clamp pitch to prevent upside down
        if (pitch > PI_HALF) {
            pitch = PI_HALF;
        } else if (pitch < -PI_HALF) {
            pitch = -PI_HALF;
        }

        // recalculate waling direction
        camera_dir[0] = sin(yaw);
        camera_dir[2] = -cos(yaw);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera_pos[0] += camera_dir[0] * walk_speed * delta_time;
            camera_pos[2] += camera_dir[2] * walk_speed * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera_pos[0] -= camera_dir[0] * walk_speed * delta_time;
            camera_pos[2] -= camera_dir[2] * walk_speed * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera_pos[0] += camera_dir[2] * walk_speed * delta_time;
            camera_pos[2] -= camera_dir[0] * walk_speed * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera_pos[0] -= camera_dir[2] * walk_speed * delta_time;
            camera_pos[2] += camera_dir[0] * walk_speed * delta_time;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera_pos[1] += walk_speed * delta_time;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera_pos[1] -= walk_speed * delta_time;
        }

        // recalculate matrices
        set_view_matrix4(yaw, pitch, -camera_pos[0], -camera_pos[1], -camera_pos[2], viewMatrix);

        // clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // load matrizes
        glUseProgram(programID);
        glUniformMatrix4fv(viewID, 1, GL_FALSE, viewMatrix);
        glUniformMatrix4fv(projectionID, 1, GL_FALSE, projectionMatrix);
        glUniform3fv(cameraID, 1, camera_pos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, demo_texture);
        glUniform1i(textureID, 0);

        render(obj1, modelID, modelMatrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, demo_texture2);
        glUniform1i(textureID, 0);

        render(obj2, modelID, modelMatrix);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    unload_render_object(obj1);
    unload_render_object(obj2);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &vertexArrayID);

    glfwTerminate();
    return 0;
}
