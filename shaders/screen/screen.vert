//default screen vertex shader

#version 330 core

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec2 vTexCoord; //vertex input texture coordinate

out vec2 fTexCoord; //fragment output texture coordinate

void main() {
    gl_Position = vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader

    fTexCoord = vTexCoord; //pass the texture coordinate to the fragment shader
}