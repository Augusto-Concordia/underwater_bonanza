//default lit fragment shader

#version 330 core

//	Voronoi 3D Noise
//	https://github.com/MaxBittker/glsl-voronoi-noise/blob/master/3d.glsl

const mat2 myt = mat2(.92121212, .13131313, -.13131313, .82121212);
const vec2 mys = vec2(1e4, 1e6);

vec2 rhash(vec2 uv) {
    uv *= myt;
    uv *= mys;
    return fract(fract(mys / uv) * uv);
}

vec3 hash(vec3 p) {
    return fract(
    sin(vec3(dot(p, vec3(1.0, 57.0, 113.0)), dot(p, vec3(57.0, 113.0, 1.0)),
    dot(p, vec3(113.0, 1.0, 57.0)))) *
    43758.5453);
}

vec3 voronoi3d(const in vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);

    float id = 0.0;
    vec2 res = vec2(100.0);
    for (int k = -1; k <= 1; k++) {
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                vec3 b = vec3(float(i), float(j), float(k));
                vec3 r = vec3(b) - f + hash(p + b);
                float d = dot(r, r);

                float cond = max(sign(res.x - d), 0.0);
                float nCond = 1.0 - cond;

                float cond2 = nCond * max(sign(res.y - d), 0.0);
                float nCond2 = 1.0 - cond2;

                id = (dot(p + b, vec3(1.0, 57.0, 113.0)) * cond) + (id * nCond);
                res = vec2(d, res.x) * cond + res * nCond;

                res.y = cond2 * d + nCond2 * res.y;
            }
        }
    }

    return vec3(sqrt(res), abs(id));
}

struct Light {
    vec3 position;
    vec3 target;
    vec3 color;

    int type;
    float shadows_influence;
    vec3 attenuation;

    // a light generally does not have information on how it's going to affect the objects, but this is a shortcut
    float ambient_strength;
    float diffuse_strength;
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

float Map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float CalculateShadowScalar(int index, vec4 fragPosLightSpace, float influence, vec3 norm, vec3 lightDir) {
    //shadow calculation
    vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - max(0.000175 * (1.0 - dot(norm, lightDir)), 0.000025)) < closestDepth ? 1.0 : influence; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

vec3 CalculateDirectionalLight(Light light, vec4 fragPosLightSpace, int index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightTargetVec = light.position - light.target;
    vec3 lightTargetDir = normalize(lightTargetVec); // this is used for lighting calculations, because it's a directional light, so the position of the light does not matter
    float lightDistance = dot(light.position - WorldPos, lightTargetDir);

    float diffFactor = max(dot(norm, lightTargetDir), 0.0);
    vec3 diffuse = diffFactor * light.diffuse_strength * light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightTargetDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * light.specular_strength * light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(index, fragPosLightSpace, light.shadows_influence, norm, lightTargetDir);

    //caustics calculation
    vec3 causticsFactor = voronoi3d(vec3(fragPosLightSpace.x, fragPosLightSpace.y, u_time * 0.04f) * 10.0f);
    vec3 caustics = causticsFactor.z * (max(step((causticsFactor.x) * (causticsFactor.y), 0.4) - step((causticsFactor.x) * (causticsFactor.y), 0.38), 0.0)) * light.color;

    vec3 colorResult = (caustics + diffuse + specular) * //lighting
    shadowScalar * //shadows
    2.0 / (light.attenuation.x + light.attenuation.y * lightDistance + light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
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