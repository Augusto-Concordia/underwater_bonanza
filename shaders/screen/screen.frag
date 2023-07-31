//default screen fragment shader

#version 330 core

uniform vec3 u_color; //screen color
uniform float u_alpha; //screen opacity

uniform sampler2D u_texture; //light screen depth texture

in vec2 fTexCoord; //texture coordinates

out vec4 outColor; //rgba color output

// standard value mapping function
vec3 map(vec3 value, vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

//entrypoint
void main() {
    // for better depth visibility when testing the shader
    //outColor = vec4(map(texture(u_texture, fTexCoord).rrr, vec3(0.95), vec3(1.0), vec3(0.0), vec3(1.0)), 1.0); //texture(u_texture, fTexCoord); //vec4(fTexCoord, 1.0, u_alpha);

    outColor = texture(u_texture, fTexCoord);
}