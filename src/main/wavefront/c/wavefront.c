#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "wavefront.h"

int count_arguments(char *line) {
    int i;
    int len   = strlen(line);
    int count = 0;
    for (i = 0; i < len; i++) {
        if (line[i] == ' ') {
            count++;
        }
    }
    return count;
}

void extract_floats(char *line, float *values) {
    char *current_arg = line;
    char *next_arg;
    current_arg = strchr(current_arg, ' ') + 1; // skip prefix
    int i       = 0;
    while (current_arg) {
        next_arg = strchr(current_arg, ' ');
        if (next_arg) {
            *next_arg = '\0';
        }
        // handle arg
        values[i] = atof(current_arg);
        // restore line
        if (next_arg) {
            *next_arg = ' ';
        }
        current_arg = next_arg ? (next_arg + 1) : NULL;
        i++;
    }
}

void extract_slashed_indices(char *line, int *values) {
    char *current_arg = line;
    char *next_arg;
    int i = 0;
    while (current_arg) {
        next_arg = strchr(current_arg, '/');
        if (next_arg) {
            *next_arg = '\0';
        }
        // handle arg
        values[i] = (int)atof(current_arg);
        // restore line
        if (next_arg) {
            *next_arg = '/';
        }
        current_arg = next_arg ? (next_arg + 1) : NULL;
        i++;
    }
}

void extract_indices(char *line, int *values) {
    char *current_arg = line;
    char *next_arg;
    current_arg = strchr(current_arg, ' ') + 1; // skip prefix
    int i       = 0;
    while (current_arg) {
        next_arg = strchr(current_arg, ' ');
        if (next_arg) {
            *next_arg = '\0';
        }
        // handle arg
        extract_slashed_indices(current_arg, values + i);
        // restore line
        if (next_arg) {
            *next_arg = ' ';
        }
        current_arg = next_arg ? (next_arg + 1) : NULL;
        i += 3;
    }
}

float *loadWavefrontObj(char *file_path, int *loaded_faces) {
    printf("Importing wavefront object from %s\n", file_path);
    int i;

    int count_obj     = 0;
    int count_vert    = 0;
    int count_face    = 0;
    int count_normal  = 0;
    int count_texture = 0;

    char *obj_contents = read_contents(file_path);
    char *current_line = obj_contents;
    char *next_line;

    int arg_count;
    int line_number = 1;
    // pass 1: verify file and gather information
    while (current_line) {
        next_line = strchr(current_line, '\n');
        if (next_line) {
            *next_line = '\0';
        }
        // handle line
        if (current_line[0] == 'o') {
            count_obj++;
            if (count_obj > 1) {
                printf("Expected exactly one object, but got a second one in line %d\n", line_number);
                return NULL;
            }
        }
        if (current_line[0] == 'v') {
            if (current_line[1] == ' ') {
                count_vert++;
            } else if (current_line[1] == 'n') {
                count_normal++;
            } else if (current_line[1] == 't') {
                count_texture++;
            }
        }
        if (current_line[0] == 'f') {
            count_face++;
            arg_count = count_arguments(current_line);
            if (arg_count != 3) {
                printf("Expected triangulated faces, but got a face with %d vertices "
                       "in line %d\n",
                       arg_count, line_number);
                return NULL;
            }
        }
        // restore line
        if (next_line) {
            *next_line = '\n';
        }
        current_line = next_line ? (next_line + 1) : NULL;
        line_number++;
    }
    // verify one object found
    if (count_obj == 0) {
        printf("No object present.\n");
        return NULL;
    }

    printf("vertices: %d, normals: %d, texture coordinates: %d, faces: %d\n", count_vert, count_normal, count_texture, count_face);

    // allocate memory
    float *vertices = malloc(count_vert * 3 * sizeof(float));
    memset(vertices, 0, count_vert * 3 * sizeof(float));

    float *normals = malloc(count_normal * 3 * sizeof(float));
    memset(normals, 0, count_normal * 3 * sizeof(float));

    float *textures = malloc(count_texture * 2 * sizeof(float));
    memset(textures, 0, count_texture * 2 * sizeof(float));

    int *indice_values = malloc(9 * sizeof(int));
    memset(indice_values, 0, 9 * sizeof(int));

    float *float_values = malloc(3 * sizeof(float));
    memset(float_values, 0, 3 * sizeof(float));

    // store vertices: 1 face = (3 vertices + 3 normals + 2 text_coords) * 3
    // floats = 24 floats
    float *vert_buffer = malloc(count_face * 24 * sizeof(float));
    memset(vert_buffer, 0, count_face * 24 * sizeof(float));

    // pass 2: read data
    line_number       = 0;
    current_line      = obj_contents;
    int vert_index    = 0;
    int normal_index  = 0;
    int texture_index = 0;
    int buf_index     = 0;
    int vert_ref_index;
    int normal_ref_index;
    int texture_ref_index;
    while (current_line) {
        next_line = strchr(current_line, '\n');
        if (next_line) {
            *next_line = '\0';
        }
        // handle line
        if (current_line[0] == 'v') {
            if (current_line[1] == ' ') {
                // vertex
                extract_floats(current_line, float_values);
                for (i = 0; i < 3; i++) {
                    vertices[vert_index++] = float_values[i];
                }
            } else if (current_line[1] == 'n') {
                // vertex normal
                extract_floats(current_line, float_values);
                for (i = 0; i < 3; i++) {
                    normals[normal_index++] = float_values[i];
                }
            } else if (current_line[1] == 't') {
                // vertex texture
                extract_floats(current_line, float_values);
                // texture coordinates 2D
                for (i = 0; i < 2; i++) {
                    textures[texture_index++] = float_values[i];
                }
            }
        } else if (current_line[0] == 'f') {
            extract_indices(current_line, indice_values);
            for (i = 0; i < 3; i++) {
                // vertex data
                vert_ref_index           = (indice_values[i * 3 + 0] - 1) * 3;
                vert_buffer[buf_index++] = vertices[vert_ref_index + 0];
                vert_buffer[buf_index++] = vertices[vert_ref_index + 1];
                vert_buffer[buf_index++] = vertices[vert_ref_index + 2];
                // normals
                normal_ref_index         = (indice_values[i * 3 + 2] - 1) * 3;
                vert_buffer[buf_index++] = normals[normal_ref_index + 0];
                vert_buffer[buf_index++] = normals[normal_ref_index + 1];
                vert_buffer[buf_index++] = normals[normal_ref_index + 2];
                // texture coordinates
                texture_ref_index        = (indice_values[i * 3 + 1] - 1) * 2;
                vert_buffer[buf_index++] = textures[texture_ref_index + 0];
                vert_buffer[buf_index++] = textures[texture_ref_index + 1];
            }
        }
        // restore line
        if (next_line) {
            *next_line = '\n';
        }
        current_line = next_line ? (next_line + 1) : NULL;
        line_number++;
    }
    free(vertices);
    free(normals);
    free(textures);
    free(indice_values);
    free(float_values);
    // store loaded faces and return vertex data
    *loaded_faces = count_face;
    return vert_buffer;
}
