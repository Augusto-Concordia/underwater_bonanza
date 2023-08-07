//default lit fragment shader

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

uniform vec3 u_cam_pos; //cam position

uniform Light u_lights[4];
uniform sampler2DArray u_light_depth_textures;

uniform vec3 u_color; //color
uniform float u_alpha; //opacity
uniform int u_shininess; //material shininess

uniform float u_texture_influence = 0.5; //are textures enabled?

uniform sampler2D u_texture; //object texture

in vec3 Normal; //normal of the fragment
in vec3 WorldPos; //position of the fragment in world space
in vec4 FragPosLightSpace[4]; //light space position of the fragment
in vec2 FragUv; //uv of the fragment

layout(location = 0) out vec4 out_color; //rgba color output
layout(location = 1) out vec4 camera_pos; //camera-space position output
layout(location = 2) out vec4 world_pos; //world-space position output

float calculateShadowScalar(int index, vec4 fragPosLightSpace, float influence, vec3 norm, vec3 lightDir) {
    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - max(0.000100 * (1.0 - dot(norm, lightDir)), 0.000025)) < closestDepth ? 1.0 : influence; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

vec3 calculateSpotLight(Light light, vec4 fragPosLightSpace, int index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - WorldPos);
    float lightDistance = length(light.position - WorldPos);

    //spotlight calculation
    float theta = dot(lightDir, light.spot_dir);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * light.specular_strength * light.color;

    float shadowScalar = calculateShadowScalar(index, fragPosLightSpace, light.shadows_influence, norm, lightDir);

    vec3 colorResult = (diffuse + specular) * max(theta - light.spot_cutoff, 0.0) * //spotlight
                            shadowScalar * //shadows
                            2.0 / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
}

vec3 calculatePointLight(Light light, vec4 fragPosLightSpace, int index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - WorldPos);
    float lightDistance = length(light.position - WorldPos);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * light.specular_strength * light.color;

    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    float shadowScalar = (currentDepth - max(0.000100 * (1.0 - dot(norm, lightDir)), 0.000025)) < closestDepth ? 1.0 : light.shadows_influence; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

    vec3 colorResult = (diffuse + specular) * //spotlight
    shadowScalar * //shadows
    2.0 / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
}

//entrypoint
void main() {
    vec3 approximateAmbient;
    vec3 lightsColor;

    for (int i = 0; i < u_lights.length(); i++) {
        approximateAmbient += (u_lights[i].ambient_strength * u_lights[i].color);

        if (u_lights[i].point_spot_influence < 0.5) //point light
            lightsColor += calculatePointLight(u_lights[i], FragPosLightSpace[i], i);
        else //spotlight
            lightsColor += calculateSpotLight(u_lights[i], FragPosLightSpace[i], i);
    }

    approximateAmbient = approximateAmbient / u_lights.length();

    vec3 colorResult = (approximateAmbient + lightsColor) * vec3(mix(vec4(u_color, 1.0), texture(u_texture, FragUv), u_texture_influence)); //pure color or texture, mixed with lighting

    out_color = vec4(colorResult, u_alpha);
    camera_pos = vec4(gl_FragCoord.xyz / gl_FragCoord.w, 1.0); //true depth output
    world_pos = vec4(WorldPos, 1.0); //true position output
}