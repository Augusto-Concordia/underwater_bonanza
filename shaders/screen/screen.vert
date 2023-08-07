//default screen vertex shader

#version 330 core

layout (location = 0) in vec3 vPos; //vertex input position

void main() {
    gl_Position = vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader
}