//default line fragment shader

#version 330 core

uniform vec3 u_color; //grid color
uniform float u_alpha; //grid opacity

layout(location = 0) out vec4 out_color; //rgba color output
layout(location = 1) out vec4 true_depth; //true depth output

//entrypoint
void main() {
    true_depth = vec4(vec3(gl_FragCoord.z / gl_FragCoord.w), 1.0); //true depth output
    out_color = vec4(u_color, u_alpha);
}