//default vertex shader

#version 410 core

layout (location = 0) in vec3 vPos; //vertex input position
layout (location = 1) in vec3 vColor; //color input of each vertex
layout (location = 2) in vec2 vTexCoord; //texture map of each vertex

out vec3 vertexColor; //rgb color output for this vertex
out vec2 texCoord; //texture coordinate output for this vertex

uniform mat4 u_model_transform; //model matrix
uniform mat4 u_view_projection; //view projection matrix

void main() {
    gl_Position = u_view_projection * u_model_transform * vec4(vPos, 1.0); //gl_Position is a built-in property of a vertex shader
    vertexColor = vColor; //sets the color of this vertex
    texCoord = vTexCoord; //sets the texture coordinates of this vertex
}