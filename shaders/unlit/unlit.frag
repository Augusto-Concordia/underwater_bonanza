//default unlit fragment shader

#version 330 core

uniform vec3 u_color; //color
uniform float u_alpha; //opacity

uniform float u_texture_influence = 0.0; //are textures enabled?
uniform float u_time; //time

uniform sampler2D u_texture; //object texture

in vec2 FragUv;
in vec3 WorldPos;

layout(location = 0) out vec4 out_color; //rgba color output
layout(location = 1) out vec4 camera_pos; //camera-space position output
layout(location = 2) out vec4 world_pos; //world-space position output

//entrypoint
void main() {
    float wobbleAmount = sin(u_time+WorldPos.y*30) * 0.01f;
    vec2 wobbleOffset = vec2(wobbleAmount, 0.0f);
    vec2 finalUV = FragUv+wobbleOffset;
    //finalUV.x = clmap(finalUV.x, 0.0f, 1.0f);
    out_color = mix(vec4(u_color, u_alpha), texture(u_texture, finalUV)*vec4(1.0f, 1.0f, 1.0f, u_alpha), u_texture_influence);
    camera_pos = vec4(gl_FragCoord.xyz / gl_FragCoord.w, 1.0); //true depth output
    world_pos = vec4(WorldPos, 1.0); //true position output
}