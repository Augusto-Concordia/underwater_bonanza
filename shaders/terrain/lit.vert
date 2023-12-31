//default lit vertex shader

#version 330 core

#define LIGHT_COUNT 4

struct Light {
    vec3 position;
    vec3 target;
    vec3 color;

    int type;
    float shadows_influence;
    vec3 attenuation;

    // a light generally does not have information on how it's going to affect the objects, but this is a shortcut
    float ambient_strength;
    float diffuse_strength;
    float specular_strength;

    float spot_cutoff;

    mat4 light_view_projection;
};

uniform Light u_lights[LIGHT_COUNT];

uniform mat4 u_model_transform; //model matrix
uniform mat4 u_view_projection; //view projection matrix

uniform vec2 u_texture_tiling; //texture (uv) tiling

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec3 vNormal; //vertex input normal
layout (location = 2) in vec2 vUv; //vertex input uv

out vec3 Normal;
out vec3 WorldPos;
out vec4 FragPosLightSpace[LIGHT_COUNT];
out vec2 FragUv;

void main() {
    Normal = mat3(transpose(inverse(u_model_transform))) * vNormal; //we need to transform the normal with the normal matrix (https://learnopengl.com/Lighting/Basic-Lighting & http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/)

    WorldPos = vec3(u_model_transform * vec4(vPos, 1.0));

    for (int i = 0; i < u_lights.length(); i++) {
        FragPosLightSpace[i] = u_lights[i].light_view_projection * vec4(WorldPos, 1.0);
    }

    FragUv = vUv / u_texture_tiling;

    gl_Position = u_view_projection * vec4(WorldPos, 1.0); //gl_Position is a built-in property of a vertex shader
}