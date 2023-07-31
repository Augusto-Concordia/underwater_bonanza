//default grid fragment shader

#version 330 core

uniform vec3 u_color; //grid color
uniform float u_alpha; //grid opacity

layout(location = 0) out vec4 out_color; //rgba color output

//entrypoint
void main() {
    out_color = vec4(u_color, u_alpha);
}