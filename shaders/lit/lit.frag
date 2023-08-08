//default lit fragment shader

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

uniform float u_time; //time
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

float CalculateShadowScalar(int index, vec4 fragPosLightSpace, float influence, vec3 norm, vec3 lightDir) {
    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - max(0.000100 * (1.0 - dot(norm, lightDir)), 0.000025)) < closestDepth ? 1.0 : influence; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

vec3 CalculateDirectionalLight(Light light, vec4 fragPosLightSpace, int index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightTargetVec = light.position - light.target;
    vec3 lightDir = normalize(light.position - WorldPos);
    float lightDistance = dot(lightTargetVec, light.position - WorldPos);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * light.specular_strength * light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(index, fragPosLightSpace, light.shadows_influence, norm, lightDir);
    shadowScalar += noise1(vec2(WorldPos.x, WorldPos.y) * u_time / 100.0f) * 0.1;

    vec3 colorResult = (diffuse + specular) * //lighting
    shadowScalar * //shadows
    2.0f / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance); //attenuation

    return vec3(shadowScalar);
}

vec3 CalculateSpotLight(Light light, vec4 fragPosLightSpace, int index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightTargetDir = normalize(light.position - light.target);
    vec3 lightDir = normalize(light.position - WorldPos);
    float lightDistance = length(light.position - WorldPos);

    //spotlight calculation
    float theta = dot(lightDir, lightTargetDir);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * light.specular_strength * light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(index, fragPosLightSpace, light.shadows_influence, norm, lightDir);

    vec3 colorResult = (diffuse + specular) * max(theta - light.spot_cutoff, 0.0) * //spotlight
                            shadowScalar * //shadows
                            2.0 / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
}

vec3 CalculatePointLight(Light light, vec4 fragPosLightSpace, int index) {
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
    float shadowScalar = CalculateShadowScalar(index, fragPosLightSpace, light.shadows_influence, norm, lightDir);

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

        if (u_lights[i].type == 0) //directional
            lightsColor += CalculateDirectionalLight(u_lights[i], FragPosLightSpace[i], i);
        else if (u_lights[i].type == 1)//point light
            lightsColor += CalculatePointLight(u_lights[i], FragPosLightSpace[i], i);
        else if (u_lights[i].type == 2) //spotlight
            lightsColor += CalculateSpotLight(u_lights[i], FragPosLightSpace[i], i);
    }

    approximateAmbient = approximateAmbient / u_lights.length();

    vec3 colorResult = (approximateAmbient + lightsColor) * vec3(mix(vec4(u_color, 1.0), texture(u_texture, FragUv), u_texture_influence)); //pure color or texture, mixed with lighting

    out_color = vec4(colorResult, u_alpha);
    camera_pos = vec4(gl_FragCoord.xyz / gl_FragCoord.w, 1.0); //true depth output
    world_pos = vec4(WorldPos, 1.0); //true position output
}