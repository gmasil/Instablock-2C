#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

out vec3 fragment_normal;
out vec2 fragment_tex;
out vec3 eye;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camera_pos;

void main(){
    gl_Position = projection * view * model * vec4(position, 1);
    eye = normalize((model * vec4(position, 1)).xyz - camera_pos);
    fragment_normal = normalize(normal);
    fragment_tex = tex;
}
