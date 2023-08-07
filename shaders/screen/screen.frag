//default screen fragment shader

#version 330 core

struct Light {
    vec3 position;
    vec3 color;

    float point_spot_influence;
    float shadows_influence;
    vec3 attenuation;

    float ambient_strength;
    float specular_strength;

    vec3 spot_dir;
    float spot_cutoff;

    mat4 light_view_projection;
};

uniform Light u_lights[4]; //lights array

uniform vec3 u_cam_pos; //camera position

uniform vec3 u_fog_color = vec3(0.0, 0.36, 0.73); //fog color

uniform sampler2D u_scene_color_texture; //scene render texture
uniform sampler2D u_scene_true_depth_texture; //scene position texture
uniform sampler2D u_scene_true_pos_texture; //scene position texture
uniform sampler2DArray u_light_depth_textures; //light depth textures (from lights' perspective)

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

float calculateShadowScalar(int index, vec4 fragPosLightSpace) {
    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float shadowMapDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - 0.00100) < shadowMapDepth ? 1.0 : 0.0; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

// Mie scaterring approximated with Henyey-Greenstein phase function.
float ComputeScattering(float lightDotView)
{
    const float G_SCATTERING = 0.76f;
    const float PI = 3.1415926535897932384626433832795f;

    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));
    return result;
}

//entrypoint
void main() {
    vec4 sceneColor = texture(u_scene_color_texture, FragUv);
    float trueDepth = texture(u_scene_true_depth_texture, FragUv).z; // true depth is the z component of each fragment
    vec3 truePos = texture(u_scene_true_pos_texture, FragUv).xyz;

    // if the depth is too small, it's probably a background pixel
    // so we just color it the fog color
    if (trueDepth < 0.00001) {
        OutColor = vec4(u_fog_color, 1.0);
        return;
    }

    // calculate fog factor
    float fogFactor = map(trueDepth, 0.0, 50.0, 0.0, 1.0);
    fogFactor = 1.0 / pow(exp(fogFactor * 0.66), 2.0); // exponential fog

    int volumetricSteps = 8; // number of steps to take when raymarching
    vec3 volumetricRay = truePos - u_cam_pos; // ray from camera to fragment
    vec3 currentPos = u_cam_pos; // current position in the raymarch

    vec3 test;

    float shadowScalar = 0.0;

    // raymarch through the fog
    for (int i = 0; i < volumetricSteps; i++) {
        for(int j = 0; j < u_lights.length(); j++) {
            Light light = u_lights[j];

            // calculate the fragment position in light space
            vec4 fragPosLightSpace = light.light_view_projection * vec4(currentPos, 1.0);

            // calculate the shadow scalar for the current light
            shadowScalar = calculateShadowScalar(j, fragPosLightSpace);

            // calculate the light direction
            vec3 lightDir = normalize(light.position - currentPos);
            vec3 viewDir = normalize(volumetricRay);

            if (shadowScalar > 0.5) {
                //fogFactor -= 0.01;
                test += ComputeScattering(dot(viewDir, lightDir)) * light.color;
            }
        }

        //test /= float(u_lights.length());

        // calculate the next step
        currentPos += volumetricRay / float(volumetricSteps);
    }

    // mix fog color with scene color
    OutColor = vec4(test / float(volumetricSteps), 1.0);// mix(vec4(u_fog_color, 1.0), sceneColor, clamp(fogFactor, 0.0, 1.0));
}