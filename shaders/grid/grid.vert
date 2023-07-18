//default grid vertex shader

#version 410 core

uniform mat4 u_model_transform; //model matrix
uniform mat4 u_view_projection; //view projection matrix

layout (location = 0) in vec3 vPos; //vertex input position

void main() {
    gl_Position = u_view_projection * u_model_transform * vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader
}