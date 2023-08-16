#version 330 core

out vec3 TexCoords;


uniform mat4 u_model_transform; //model matrix
uniform mat4 u_view_projection; //view projection matrix

uniform vec2 u_texture_tiling; //texture (uv) tiling

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec3 vNormal; //vertex input normal
layout (location = 2) in vec2 vUv; //vertex input uv

out vec2 FragUv;
out vec3 WorldPos;

void main() {
    TexCoords = vPos;
    gl_Position = u_view_projection * vec4(vPos, 1.0);

    FragUv = vUv / u_texture_tiling;
    WorldPos = vPos;
}