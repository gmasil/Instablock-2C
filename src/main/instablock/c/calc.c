#define M_PI 3.14159265358979323846
#define RADIANS_TO_DEGREES 57.2957795130823208768
#define DEGREES_TO_RADIANS 0.01745329251994329577

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc.h"

/**
 * Create a vector 3 with given values
 */
float *create_vector3(float x, float y, float z) {
    float *mat = malloc(3 * sizeof(float));

    mat[0] = x;
    mat[1] = y;
    mat[2] = z;
    return mat;
}

/**
 * Create a vector 4 with given values
 */
float *create_vector4(float x, float y, float z, float w) {
    float *mat = malloc(4 * sizeof(float));

    mat[0] = x;
    mat[1] = y;
    mat[2] = z;
    mat[3] = w;
    return mat;
}

/**
 * Create a 4x4 matrix filled with zeros.
 */
float *create_matrix4(void) {
    float *mat = malloc(16 * sizeof(float));
    memset(mat, 0, 16 * sizeof(float));
    return mat;
}

/**
 * Create a 4x4 matrix for translation and store values in mat
 */
void set_translation_matrix4(float x, float y, float z, float *mat) {
    memset(mat, 0, 4 * 4 * sizeof(float));
    // diagonal
    mat[0]  = 1.0f;
    mat[5]  = 1.0f;
    mat[10] = 1.0f;
    mat[15] = 1.0f;
    // x, y ,z
    mat[12] = x;
    mat[13] = y;
    mat[14] = z;
}

/**
 * Create a rotation matrix of given yaw and pitch and store it to existing 4x4
 * matrix mat.
 *
 * Args:
 * yaw:    look left/right
 * pitch:  look up/down
 * mat:    the 4x4 matrix to store values in
 */
void set_rotation_matrix4(float yaw, float pitch, float *mat) {
    memset(mat, 0, 4 * 4 * sizeof(float));

    // Matrix indices
    //  0  4  8 12
    //  1  5  9 13
    //  2  6 10 14
    //  3  7 11 15

    // https://en.wikipedia.org/wiki/Rotation_matrix
    // beta and gamma resemble angles in wikipedia, yaw and pitch is 3D space
    // correct
    float sin_beta  = sin(yaw);
    float cos_beta  = cos(yaw);
    float sin_gamma = sin(pitch);
    float cos_gamma = cos(pitch);

    mat[0] = cos_beta;
    mat[8] = sin_beta;

    mat[1] = -sin_beta * -sin_gamma;
    mat[5] = cos_gamma;
    mat[9] = cos_beta * -sin_gamma;

    mat[2]  = -sin_beta * cos_gamma;
    mat[6]  = sin_gamma;
    mat[10] = cos_beta * cos_gamma;

    mat[15] = 1.0f;
}

/**
 * Create a view matrix of given yaw, pitch and camera position and store it to
 * existing 4x4 matrix mat.
 *
 * Args:
 * yaw:    look left/right
 * pitch:  look up/down
 * mat:    the 4x4 matrix to store values in
 */
void set_view_matrix4(float yaw, float pitch, float x, float y, float z, float *mat) {
    memset(mat, 0, 4 * 4 * sizeof(float));

    // Matrix indices
    //  0  4  8 12
    //  1  5  9 13
    //  2  6 10 14
    //  3  7 11 15

    // https://en.wikipedia.org/wiki/Rotation_matrix
    // beta and gamma resemble angles in wikipedia, yaw and pitch is 3D space
    // correct
    float sin_beta  = sin(yaw);
    float cos_beta  = cos(yaw);
    float sin_gamma = sin(pitch);
    float cos_gamma = cos(pitch);

    mat[0]  = cos_beta;
    mat[8]  = sin_beta;
    mat[12] = x * mat[0] + z * mat[8];

    mat[1]  = -sin_beta * -sin_gamma;
    mat[5]  = cos_gamma;
    mat[9]  = cos_beta * -sin_gamma;
    mat[13] = x * mat[1] + y * mat[5] + z * mat[9];

    mat[2]  = -sin_beta * cos_gamma;
    mat[6]  = sin_gamma;
    mat[10] = cos_beta * cos_gamma;
    mat[14] = x * mat[2] + y * mat[6] + z * mat[10];

    mat[15] = 1.0f;
}

/**
 * Calculate 4x4 projection matrix to use as a perspective.
 *
 * Args:
 * float angle:    Vertical field of view in radians
 * float aspect:   Aspect ratio
 * float near:     Near clipping plane
 * float far:      Far clipping plane
 */
float *perspective(float angle, float aspect, float near, float far) {
    float tan_half_angle;
    float *projectionMatrix;

    projectionMatrix = create_matrix4();
    tan_half_angle   = tan(angle / 2);

    // Matrix indices
    //  0  4  8 12
    //  1  5  9 13
    //  2  6 10 14
    //  3  7 11 15

    projectionMatrix[0]  = 1.0f / (aspect * tan_half_angle);
    projectionMatrix[5]  = 1.0f / tan_half_angle;
    projectionMatrix[10] = -(far - near) / (far - near);
    projectionMatrix[11] = -1.0f;
    projectionMatrix[14] = -(2.0f * far * near) / (far - near);

    return projectionMatrix;
}

/**
 * Convert given angle in radians to degrees
 */
float to_degrees(float radians) {
    return radians * RADIANS_TO_DEGREES;
}

/**
 * Convert given angle in degrees to radians
 */
float to_radians(float degrees) {
    return degrees * DEGREES_TO_RADIANS;
}

/**
 * Set given value in given 4x4 matrix at position (x, y)
 * Top left position is (0, 0)
 * Bottom left position is (3, 0)
 */
void set_mat4(float *mat, int x, int y, float value) {
    mat[x * 4 + y] = value;
}

/**
 * GLM lookAt
 * Align target (4x4 matrix) according to eye, center and up vectors.
 *
 * Usage example:
 * GLfloat *view = create_matrix_zero(4, 4);
 * float eye[] = { 4.0f, 3.0f ,3.0f };
 * float center[] = { 0.0f, 0.0f, 0.0f };
 * float up[] = { 0.0f, 1.0f, 0.0f };
 * lookAt(eye, center, up, view);
 */
void lookAt(float *eye, float *center, float *up, float *target) {
    float f[3], s[3], u[3];

    subtract_vector(center, eye, 3, f);
    normalize(f, 3);
    cross_product_vector3(f, up, s);
    normalize(s, 3);
    cross_product_vector3(s, f, u);

    memset(target, 0, 16 * sizeof(float));

    set_mat4(target, 0, 0, s[0]);
    set_mat4(target, 1, 0, s[1]);
    set_mat4(target, 2, 0, s[2]);

    set_mat4(target, 0, 1, u[0]);
    set_mat4(target, 1, 1, u[1]);
    set_mat4(target, 2, 1, u[2]);

    set_mat4(target, 0, 2, -f[0]);
    set_mat4(target, 1, 2, -f[1]);
    set_mat4(target, 2, 2, -f[2]);

    set_mat4(target, 3, 0, -dot_product(s, eye, 3));
    set_mat4(target, 3, 1, -dot_product(u, eye, 3));
    set_mat4(target, 3, 2, dot_product(f, eye, 3));

    set_mat4(target, 3, 3, 1);
}

/**
 * Calc inverse length of given vector with given dimension
 * Returns 1 / sqrt(a^2 + b^2 + ...)
 */
float vector_inverse_length(float *vec, int dim) {
    float sum;
    int i;
    sum = 0;
    for (i = 0; i < dim; i++) {
        sum += vec[i] * vec[i];
    }
    return Q_rsqrt(sum);
}

/**
 * Quake 3 Arena implementation of reverse square root
 */
float Q_rsqrt(float number) {
    union {
        float f;
        uint32_t i;
    } conv = {.f = number};
    conv.i = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f); // two times for higher prcision
    return conv.f;
}

/**
 * Normalize given vector by changing it in place
 */
void normalize(float *vec, int dim) {
    int i;
    float inv_len;

    inv_len = vector_inverse_length(vec, dim);

    for (i = 0; i < dim; i++) {
        vec[i] = vec[i] * inv_len;
    }
}

/**
 * Multiply 4x4 matrix with vector4 and store results to target
 */
void apply_matrix4(float *mat, float *vec, float *target) {
    target[0] = mat[0] * vec[0] + mat[4] * vec[1] + mat[8] * vec[2] + mat[12] * vec[3];
    target[1] = mat[1] * vec[0] + mat[5] * vec[1] + mat[9] * vec[2] + mat[13] * vec[3];
    target[2] = mat[2] * vec[0] + mat[6] * vec[1] + mat[10] * vec[2] + mat[14] * vec[3];
    target[3] = mat[3] * vec[0] + mat[7] * vec[1] + mat[11] * vec[2] + mat[15] * vec[3];
}

/**
 * Multiply 3x3 matrix with vector3 and store results to target
 */
void apply_matrix3(float *mat, float *vec, float *target) {
    target[0] = mat[0] * vec[0] + mat[3] * vec[1] + mat[6] * vec[2];
    target[1] = mat[1] * vec[0] + mat[4] * vec[1] + mat[7] * vec[2];
    target[2] = mat[2] * vec[0] + mat[5] * vec[1] + mat[8] * vec[2];
}

/**
 * Subtract two vectors of given dimension and store result into target.
 * target = vec1 - vec2
 */
void subtract_vector(float *vec1, float *vec2, int dim, float *target) {
    int i;
    for (i = 0; i < dim; i++) {
        target[i] = vec1[i] - vec2[i];
    }
}

/**
 * Add two vectors of given dimension and store result into target.
 * target = vec1 + vec2
 */
void add_vector(float *vec1, float *vec2, int dim, float *target) {
    int i;
    for (i = 0; i < dim; i++) {
        target[i] = vec1[i] + vec2[i];
    }
}

/**
 * Calculate cross product of two 3D vectors and store result in target.
 */
void cross_product_vector3(float *vec1, float *vec2, float *target) {
    target[0] = ((vec1[1] * vec2[2]) - (vec1[2] * vec2[1]));
    target[1] = ((vec1[2] * vec2[0]) - (vec1[0] * vec2[2]));
    target[2] = ((vec1[0] * vec2[1]) - (vec1[1] * vec2[0]));
}

/**
 * Calculate dot product (scalar) of two vectors with given dimensioon.
 * Returns the value.
 */
float dot_product(float *vec1, float *vec2, int dim) {
    int i;
    float sum = 0;

    for (i = 0; i < dim; i++) {
        sum += vec1[i] * vec2[i];
    }

    return sum;
}

/**
 * Print given 3x3 matrix
 */
void print_matrix3(float *mat) {
    printf("( %05.1f %05.1f %05.1f\n", mat[0], mat[3], mat[6]);
    printf("  %05.1f %05.1f %05.1f\n", mat[1], mat[4], mat[7]);
    printf("  %05.1f %05.1f %05.1f )\n", mat[2], mat[5], mat[8]);
}

/**
 * Print given 4x4 matrix
 */
void print_matrix4(float *mat) {
    printf("( %05.1f %05.1f %05.1f %05.1f\n", mat[0], mat[4], mat[8], mat[12]);
    printf("  %05.1f %05.1f %05.1f %05.1f\n", mat[1], mat[5], mat[9], mat[13]);
    printf("  %05.1f %05.1f %05.1f %05.1f\n", mat[2], mat[6], mat[10], mat[14]);
    printf("  %05.1f %05.1f %05.1f %05.1f )\n", mat[3], mat[7], mat[11], mat[15]);
}

/**
 * Print given vector 3
 */
void print_vector3(float *vec) {
    printf("( %05.1f %05.1f %05.1f )\n", vec[0], vec[1], vec[2]);
}

/**
 * Print given vector 4
 */
void print_vector4(float *vec) {
    printf("( %05.1f %05.1f %05.1f %05.1f )\n", vec[0], vec[1], vec[2], vec[3]);
}
