//default lit fragment shader

#version 330 core

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

uniform Light u_lights[1];
uniform sampler2DArray u_light_depth_textures;

uniform vec3 u_color = vec3(1.0f); //color
uniform float u_alpha; //opacity
uniform int u_shininess; //material shininess

uniform float u_texture_influence = 0.5; //are textures enabled?

uniform sampler2D u_texture; //object texture

uniform int u_caustics_texture_count = 32; //number of caustics textures
uniform sampler2DArray u_caustics_texture; //caustics texture

in vec3 Normal; //normal of the fragment
in vec3 WorldPos; //position of the fragment in world space
in vec4 FragPosLightSpace[1]; //light space position of the fragment
in vec2 FragUv; //uv of the fragment

layout(location = 0) out vec4 out_color; //rgba color output
layout(location = 1) out vec4 camera_pos; //camera-space position output
layout(location = 2) out vec4 world_pos; //world-space position output

float Map(float _value, float _min1, float _max1, float _min2, float _max2) {
    return _min2 + (_value - _min1) * (_max2 - _min2) / (_max1 - _min1);
}

float CalculateShadowScalar(int _index, vec4 _fragPosLightSpace, float _influence, vec3 _norm, vec3 _lightDir) {
    //shadow calculation
    vec3 projectedCoords = _fragPosLightSpace.xyz / _fragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_light_depth_textures, vec3(projectedCoords.xy, _index)).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    return (currentDepth - max(0.000175 * (1.0 - dot(_norm, _lightDir)), 0.000025)) < closestDepth ? 1.0 : _influence; //bias calculation comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
}

vec4 CalculateCaustics(sampler2DArray _causticsTexture, vec4 _frapPosLightSpace, int _textureCount, float _time) {
    //caustics calculation
    int discreteTime = int(_time);

    //loops through all the layers in the caustics texture array, in a periodic fashion, according to the time

    //if the time is even, the index goes up, otherwise it goes down
    int nextCausticsIndexDirection = (int(discreteTime / _textureCount) % 2) == 0 ? 1 : -1;

    //if the index goes up, it's the remainder of the time divided by the number of textures, otherwise it's the number of textures minus the remainder of the time divided by the number of textures
    int currentCausticsIndex = nextCausticsIndexDirection == 1 ? (discreteTime % _textureCount) : (_textureCount - (discreteTime % _textureCount));
    int nextCausticsIndex = currentCausticsIndex + nextCausticsIndexDirection;

    vec4 currentCausticsFactor = texture(_causticsTexture, vec3(_frapPosLightSpace.x, _frapPosLightSpace.y, currentCausticsIndex));
    vec4 nextCausticsFactor = texture(_causticsTexture, vec3(_frapPosLightSpace.x, _frapPosLightSpace.y, nextCausticsIndex));

    //the mix ratio is the fractional part of the time, to make the transition between caustics textures smooth
    float causticsMixRatio = fract(u_time);

    return mix(currentCausticsFactor, nextCausticsFactor, causticsMixRatio);
}

vec3 CalculateDirectionalLight(Light _light, vec4 _fragPosLightSpace, int _index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightTargetVec = _light.position - _light.target;
    vec3 lightTargetDir = normalize(lightTargetVec); // this is used for lighting calculations, because it's a directional light, so the position of the light does not matter
    float lightDistance = dot(_light.position - WorldPos, lightTargetDir);

    float diffFactor = max(dot(norm, lightTargetDir), 0.0);
    vec3 diffuse = diffFactor * _light.diffuse_strength * _light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightTargetDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * _light.specular_strength * _light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(_index, _fragPosLightSpace, _light.shadows_influence, norm, lightTargetDir);

    //caustics calculation
    vec3 caustics = CalculateCaustics(u_caustics_texture, _fragPosLightSpace * 0.5, u_caustics_texture_count, u_time).rgb * _light.color * max(norm.y, 0.0);

    vec3 colorResult = (caustics + diffuse + specular) * //lighting
    shadowScalar * //shadows
    2.0 / (_light.attenuation.x + _light.attenuation.y * lightDistance + _light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
}

vec3 CalculateSpotLight(Light _light, vec4 _fragPosLightSpace, int _index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightTargetDir = normalize(_light.position - _light.target);
    vec3 lightDir = normalize(_light.position - WorldPos);
    float lightDistance = length(_light.position - WorldPos);

    //spotlight calculation
    float theta = dot(lightDir, lightTargetDir);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * _light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * _light.specular_strength * _light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(_index, _fragPosLightSpace, _light.shadows_influence, norm, lightDir);

    vec3 colorResult = (diffuse + specular) * max(theta - _light.spot_cutoff, 0.0) * //spotlight
                            shadowScalar * //shadows
                            2.0 / (_light.attenuation.x + _light.attenuation.y * lightDistance + _light.attenuation.z * lightDistance * lightDistance); //attenuation

    return colorResult;
}

vec3 CalculatePointLight(Light _light, vec4 _fragPosLightSpace, int _index) {
    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(_light.position - WorldPos);
    float lightDistance = length(_light.position - WorldPos);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * _light.color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - WorldPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * _light.specular_strength * _light.color;

    //shadow calculation
    float shadowScalar = CalculateShadowScalar(_index, _fragPosLightSpace, _light.shadows_influence, norm, lightDir);

    vec3 colorResult = (diffuse + specular) * //spotlight
    shadowScalar * //shadows
    2.0 / (_light.attenuation.x + _light.attenuation.y * lightDistance + _light.attenuation.z * lightDistance * lightDistance); //attenuation

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