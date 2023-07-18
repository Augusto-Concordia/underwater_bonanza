//default grid fragment shader

#version 410 core

uniform vec3 u_color; //grid color
uniform float u_alpha; //grid opacity

out vec4 fragColor; //rgba color output

//entrypoint
void main() {
    fragColor = vec4(u_color, u_alpha);
}