#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#include "load_shader.h"
#include "utils.h"

#define GL_SILENCE_DEPRECATION

GLuint load_shaders(char *vertex_file_path, char *fragment_file_path) {
    GLuint vertex_shader, fragment_shader, program;

    const char *vertex_shader_text = read_contents(vertex_file_path);
    vertex_shader                  = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    free((void *)vertex_shader_text);

    const char *fragment_shader_text = read_contents(fragment_file_path);
    fragment_shader                  = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    free((void *)fragment_shader_text);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    return program;
}
