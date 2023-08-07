//default screen vertex shader

#version 330 core

struct Light {
    vec3 position;
    vec3 color;

    float point_spot_influence;
    float shadows_influence;
    vec3 attenuation;

    float ambient_strength;
    float specular_strength;

    vec3 spot_dir;
    float spot_cutoff;

    mat4 light_view_projection;
};

uniform Light u_lights[4];

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec2 vUv; //vertex input uv

out vec3 FragPos; //fragment output position
out vec2 FragUv; //fragment output texture coordinate

void main() {
    FragPos = vPos; //pass the position to the fragment shader

    FragUv = vUv; //pass the texture coordinate to the fragment shader

    gl_Position = vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader
}