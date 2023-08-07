//default screen fragment shader

#version 330 core

uniform vec3 u_fog_color = vec3(0.0, 0.36, 0.73); //fog color
uniform float u_fog_density = 0.66; //fog density
uniform float u_fog_max_distance = 50.0; //fog max distance
uniform float u_fog_min_distance = 2.0; //fog min distance

uniform vec3 u_chromatic_aberration = vec3(-0.009, -0.006, 0.006); //chromatic aberration

uniform sampler2D u_scene_color_texture; //scene render texture
uniform sampler2D u_scene_true_depth_texture; //scene depth texture

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
    // CHROMATIC ABERRATION

    vec2 colorTexSize = textureSize(u_scene_color_texture, 0);
    vec2 colorTexCoord = gl_FragCoord.xy / colorTexSize;
    vec2 aberrationDirection = colorTexCoord - vec2(0.5);

    vec4 sceneColor;
    sceneColor.r =  texture(u_scene_color_texture, colorTexCoord + aberrationDirection * vec2(u_chromatic_aberration.r, u_chromatic_aberration.g)).r;
    sceneColor.g =  texture(u_scene_color_texture, colorTexCoord + aberrationDirection * vec2(u_chromatic_aberration.g, u_chromatic_aberration.b)).g;
    sceneColor.b =  texture(u_scene_color_texture, colorTexCoord + aberrationDirection * vec2(u_chromatic_aberration.b, u_chromatic_aberration.r)).b;
    sceneColor.a = texture(u_scene_color_texture, colorTexCoord).a;

    // FOG

    vec2 trueDepthTexSize = textureSize(u_scene_true_depth_texture, 0);
    vec2 trueDepthTexCoord = gl_FragCoord.xy / colorTexSize;
    float trueDepth = texture(u_scene_true_depth_texture, trueDepthTexCoord).r;

    // if the depth is too small, it's probably a background pixel
    // so we just color it the fog color
    if (trueDepth < 0.00001) {
        OutColor = vec4(u_fog_color, 1.0);
        return;
    }

    float fogFactor = map(trueDepth,  u_fog_min_distance, u_fog_max_distance, 0.0, 1.0);
    fogFactor = clamp(1.0 / pow(exp(fogFactor * u_fog_density), 2.0), 0.0, 1.0); // exponential fog

    vec4 foggedColor = mix(vec4(u_fog_color, 1.0), sceneColor, fogFactor);
    OutColor = foggedColor;
}