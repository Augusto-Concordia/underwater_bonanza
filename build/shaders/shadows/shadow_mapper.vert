//default shadow mapper vertex shader

#version 330 core

uniform mat4 u_model_transform; //model matrix (from the light's perspective)
uniform mat4 u_view_projection; //view projection matrix (from the light's perspective)

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec3 vNormal; //vertex input normal
layout (location = 2) in vec2 vUv; //vertex input normal

out vec3 FragPos; //fragment position in world space

void main() {
    gl_Position = u_view_projection * u_model_transform * vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader

    FragPos = vec3(u_model_transform * vec4(vPos, 1.0)); //transform fragment position to world space
}