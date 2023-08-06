//default screen fragment shader

#version 330 core

uniform vec3 u_fog_color = vec3(0.0, 0.36, 0.73); //fog color

uniform sampler2D u_scene_color_texture; //scene render texture
uniform sampler2D u_scene_true_depth_texture; //scene depth texture

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
    vec4 sceneColor = texture(u_scene_color_texture, FragUv).rgba;
    float trueDepth = texture(u_scene_true_depth_texture, FragUv).r;

    // if the depth is too small, it's probably a background pixel
    // so we just color it the fog color
    if (trueDepth < 0.00001) {
        OutColor = vec4(u_fog_color, 1.0);
        return;
    }

    float fogFactor = map(trueDepth, 2.0, 50.0, 0.0, 1.0);
    fogFactor = clamp(1.0 / pow(exp(fogFactor * 0.66), 2.0), 0.0, 1.0); // exponential fog

    OutColor = mix(vec4(u_fog_color, 1.0), sceneColor, fogFactor);
}