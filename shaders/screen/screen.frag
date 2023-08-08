//default screen fragment shader

#version 330 core

struct Light {
    vec3 position;
    vec3 target;
    vec3 color;

    int type;
    float shadows_influence;
    vec3 attenuation;

    float ambient_strength;
    float specular_strength;

    float spot_cutoff;

    mat4 light_view_projection;
};

uniform vec3 u_cam_pos; //camera position
uniform Light u_lights[4]; //lights array

uniform vec3 u_fog_color = vec3(0.0, 0.36, 0.73); //fog color
uniform float u_fog_density = 0.66; //fog density
uniform float u_fog_max_distance = 50.0; //fog max distance
uniform float u_fog_min_distance = 2.0; //fog min distance

uniform vec3 u_chromatic_aberration = vec3(-0.009, -0.006, 0.006); //chromatic aberration

uniform sampler2D u_scene_color_texture; //scene render texture
uniform sampler2D u_scene_camera_pos_texture; //scene position texture (in camera space)
uniform sampler2D u_scene_world_pos_texture; //scene position texture (in world space)
uniform sampler2DArray u_light_depth_textures; //light depth textures (from lights' perspective)

out vec4 OutColor; //rgba color output

float Map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float CalculateShadowScalar(int index, vec4 fragPosLightSpace) {
    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float shadowMapDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - 0.00100) < shadowMapDepth ? 1.0f : 0.0f; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

// Mie scaterring approximated with Henyey-Greenstein phase function.
float ComputeScattering(float lightDotView)
{
    const float G_SCATTERING = 0.55f;
    const float PI = 3.1415f;

    float result = 1.0f - G_SCATTERING * G_SCATTERING;
    result /= (4.0f * PI * pow(1.0f + G_SCATTERING * G_SCATTERING - (2.0f * G_SCATTERING) * lightDotView, 1.5f));
    return result;
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

    vec2 cameraPosTexSize = textureSize(u_scene_camera_pos_texture, 0);
    vec2 cameraPosTexCoord = gl_FragCoord.xy / colorTexSize;
    vec3 posInCameraSpace = texture(u_scene_camera_pos_texture, cameraPosTexCoord).xyz;

    vec2 worldPosTexSize = textureSize(u_scene_world_pos_texture, 0);
    vec2 worldPosTexCoord = gl_FragCoord.xy / colorTexSize;
    vec3 posInWorldSpace = texture(u_scene_world_pos_texture, worldPosTexCoord).xyz;

    // calculate fog factor
    float fogFactor = Map(posInCameraSpace.z, 0.0f, 50.0f, 0.0f, 1.0f);
    fogFactor = 1.0f / pow(exp(fogFactor * 0.66f), 2.0f); // exponential fog

    int volumetricSteps = 50; // number of steps to take when raymarching
    float volumetricDensity = 0.2f; // density of the fog

    vec3 volumetricRay = posInWorldSpace - u_cam_pos; // ray from camera to fragment
    float volumetricDist = length(volumetricRay); // distance from camera to fragment
    vec3 volumetricNormRay = normalize(volumetricRay); // ray from camera to fragment (normalized)

    float currentVolumetricDist = 0.0f; // current distance in the raymarch
    vec3 currentVolumetricPos = u_cam_pos; // current position in the raymarch

    vec3 accumulatedColor = vec3(0.0f);

    // raymarch through the fog
    for (int i = 0; i < volumetricSteps; i++) {
        vec3 lightsContribution = vec3(0.0f);

        for(int j = 0; j < u_lights.length(); j++) {
            Light light = u_lights[j];

            // calculate the fragment position in light space
            vec4 fragPosLightSpace = light.light_view_projection * vec4(currentVolumetricPos, 1.0f);

            // calculate the shadow scalar for the current light
            float shadowScalar = CalculateShadowScalar(j, fragPosLightSpace);

            // calculate the light direction
            float lightDistance = length(light.position - currentVolumetricPos);
            vec3 lightDir = normalize(light.position - currentVolumetricPos);
            vec3 lightTargetDir = normalize(light.position - light.target);

            float lightTypeScalar = 1.0f;

            if (light.type == 2) {
                lightTypeScalar = dot(lightDir, lightTargetDir);
            }

            if (shadowScalar > 0.5f) {
                lightsContribution += ComputeScattering(dot(volumetricNormRay, lightDir)) * light.color * //shadows
                lightTypeScalar *  //light type (i.e. if it's a spotlight)
                2.0f / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance) / float(u_lights.length()); //attenuation
            }
        }

        // calculate the next step
        currentVolumetricDist += volumetricDensity;
        currentVolumetricPos = u_cam_pos + currentVolumetricDist * volumetricNormRay;

        // stop if we have collided with an object
        if (currentVolumetricDist >= volumetricDist) {
            break;
        }

        accumulatedColor += lightsContribution;
    }

    accumulatedColor /= float(volumetricSteps);

    // mix fog color with scene color
    OutColor = mix(vec4(u_fog_color, 1.0f), sceneColor, clamp(fogFactor, 0.0f, 1.0f)) + vec4(accumulatedColor, 1.0f);
}