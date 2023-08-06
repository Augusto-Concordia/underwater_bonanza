//default screen fragment shader

#version 330 core

uniform sampler2D u_scene_color_texture; //scene render texture
uniform sampler2D u_scene_depth_texture; //scene depth texture

in vec3 FragPos; //fragment position
in vec2 FragUv; //texture coordinates

out vec4 OutColor; //rgba color output

// standard value mapping function
vec3 map(vec3 value, vec3 min1, vec3 max1, vec3 min2, vec3 max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

//entrypoint
void main() {
    // for better depth visibility when testing the shader
    //OutColor = vec4(map(texture(u_scene_depth_texture, FragUv).rrr, vec3(0.95), vec3(1.0), vec3(0.0), vec3(1.0)), 1.0);

    float depth = (400.0 - texture(u_scene_depth_texture, FragUv).r * 400.0) / 400.0;

    OutColor = mix(texture(u_scene_color_texture, FragUv), vec4(0.0, 0.0, 0.0, 1.0), depth);

    //OutColor = mix(texture(u_scene_color_texture, FragUv), vec4(FragUv, 1.0, 1.0), 0.5);

    //OutColor = vec4(vec3(FragUv, 1.0), 1.0);

    //OutColor = vec4(FragPos, 1.0);
}