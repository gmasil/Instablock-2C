#version 330 core

in vec3 fragment_normal;
in vec2 fragment_tex;
in vec3 eye;

out vec3 color;

uniform sampler2D tex_sampler;

void main(){
    vec3 light_direction = normalize(vec3(-0.5, -1.0, 0.3));

    float diffuse_angle = clamp(dot(-light_direction, fragment_normal), 0, 1);
    float diffuse = diffuse_angle / 2 + 0.5;

    vec3 light_reflection = reflect(light_direction, fragment_normal);
    float spec = pow(clamp(dot(-light_reflection, eye), 0, 1), 8);

    color = texture(tex_sampler, fragment_tex).rgb * (diffuse + spec);
}
