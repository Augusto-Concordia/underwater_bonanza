#version 330 core
in vec3 TexCoords; // direction vector representing a 3D texture coordinate
uniform samplerCube skybox; // cubemap texture sampler

uniform vec3 u_color; //color
uniform float u_alpha; //opacity

uniform float u_texture_influence = 0.0; //are textures enabled?
uniform float u_time; //time

uniform float wobbleFactor = 0.0f; //time

uniform sampler2D u_texture; //object texture

in vec2 FragUv;
in vec3 WorldPos;

layout(location = 0) out vec4 out_color; //rgba color output
layout(location = 1) out vec4 camera_pos; //camera-space position output
layout(location = 2) out vec4 world_pos; //world-space position output

//entrypoint
void main() {
    out_color = texture(skybox, TexCoords);

    camera_pos = vec4(gl_FragCoord.xyz / gl_FragCoord.w, 1.0); //true depth output
    world_pos = vec4(WorldPos, 1.0); //true position output
}