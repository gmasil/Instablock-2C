#ifndef CALC_H
#define CALC_H

float *create_vector3(float x, float y, float z);
float *create_vector4(float x, float y, float z, float w);
float *create_matrix4(void);
void set_translation_matrix4(float x, float y, float z, float *mat);
void set_rotation_matrix4(float yaw, float pitch, float *mat);
void set_view_matrix4(float yaw, float pitch, float x, float y, float z, float *mat);
float *perspective(float angle, float ratio, float near, float far);
float to_degrees(float radians);
float to_radians(float degrees);
void lookAt(float *eye, float *center, float *up, float *target);
float vector_inverse_length(float *vec, int dim);
float Q_rsqrt(float number);
void normalize(float *vec, int dim);
void apply_matrix4(float *mat, float *vec, float *target);
void apply_matrix3(float *mat, float *vec, float *target);
void subtract_vector(float *vec1, float *vec2, int dim, float *target);
void add_vector(float *vec1, float *vec2, int dim, float *target);
void cross_product_vector3(float *vec1, float *vec2, float *target);
float dot_product(float *vec1, float *vec2, int dim);

void print_matrix3(float *mat);
void print_matrix4(float *mat);
void print_vector3(float *vec);
void print_vector4(float *vec);

#endif // CALC_H
