#version 330 core

const float PI = 3.1415926535897932384626433832795;

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

uniform Light u_lights[4];

const float shading_ambient_strength    = 0.4;
const float shading_diffuse_strength    = 0.7;
const float shading_specular_strength   = 0.2;
const float shading_reflection_strength = 0.5f;

uniform vec3 u_cam_pos;

uniform float useMap = 0.0f;
uniform float useTexture = 1.0f;

uniform sampler2D oceanFlowMap;
uniform samplerCube skybox;

in vec3 fragment_position;
in vec3 fragment_position_model;
in vec3 fragment_position_original;
in vec4 fragment_position_light_space;
in vec3 fragment_normal;

in float light_near;
in float light_far;
in float light_cut_near;
in float light_cut_far;

in float xDerivative;
in float yDerivative;
in float zDerivative;

in float time;
in float fakeWaveUV;

in vec4 gl_FragCoord;

out vec4 result;

vec3 ambient_color(vec3 light_color_arg) {
    return shading_ambient_strength * light_color_arg;
}

vec3 diffuse_color(vec3 light_color_arg, vec3 light_position_arg, vec3 normal) {
    vec3 light_direction = normalize(u_lights[0].position - u_lights[0].target);
    return shading_diffuse_strength * light_color_arg * max(dot(normalize(normal), light_direction), 0.6f);
}

vec3 specular_color(vec3 light_color_arg, vec3 light_position_arg, vec3 normal) {
    //vec3 light_direction = normalize(light_position_arg - fragment_position);
    vec3 light_direction = normalize(light_position_arg - u_lights[0].target);
    vec3 view_direction = normalize(u_cam_pos - fragment_position);
    vec3 reflect_light_direction = reflect(-light_direction, normalize(normal));
    return shading_specular_strength * light_color_arg * pow(max(dot(reflect_light_direction, view_direction), 0.0f),32);
}

vec3 reflection_color(vec3 light_position_arg, vec3 normal)
{
    vec3 view_direction = normalize(u_cam_pos - fragment_position);
    vec3 reflect_light_direction = reflect(-view_direction, normalize(normal));

    return shading_reflection_strength * texture(skybox, reflect_light_direction).xyz;

}



void main()
{
    float heightValue = (fragment_position_model.y+1.0f)/2.0f + 0.2f;

    vec3 darkBlue = vec3(0/255.0f,51/255.0f,102/255.0f);
    vec3 lightBlue = vec3(37/255.0f,216/255.0f,255/255.0f) * 0.80f;
    vec3 oceanBlue = vec3(0/255.0f,94/255.0f,184/255.0f);
    vec3 oceanGreen = vec3(49/255.0f,100/255.0f,49/255.0f);

    vec3 normal = vec3(xDerivative, yDerivative, zDerivative);
    vec3 view = normalize(u_cam_pos - fragment_position_model);
    vec3 sun = normalize(u_lights[0].position - fragment_position_model);
    float upValue = clamp(dot(normal, sun), 0.0f, 1.0f);
    float viewValue = clamp(dot(normal, view), 0.0f, 1.0f);
    float normalLookingUp = clamp(dot(normal, vec3(0.0f, 1.0f, 0.0f)), 0.0f, 1.0f);
    float lookAtFloorValue = clamp(dot(vec3(0, 1, 0), view), 0.0f, 1.0f);

    float fragmentDistance = length(u_cam_pos - fragment_position_model);

    vec3 waterColor = mix(oceanBlue, lightBlue, (normalLookingUp) * (1-clamp(fragmentDistance/5.0f, 0.0f, 1.0f))  );

    vec2 flowSamplePos1 = vec2(fragment_position_model.x*0.2f, fragment_position_model.z*0.7f)*1.0f * (fakeWaveUV==1.0f ? 0.5f : 1.0f) + time*0.2f;
    vec2 flowSamplePos2 = vec2(fragment_position_model.x * 0.8f, fragment_position_model.z*0.6f)*2.0f * (fakeWaveUV==1.0f ? 0.5f : 1.0f) - time*0.3f;
    vec4 flowSample1 = texture( oceanFlowMap, flowSamplePos1);
    vec4 flowSample2 = texture( oceanFlowMap, flowSamplePos2);
    normal += (vec3(flowSample1.x, 0.0f, flowSample1.y)+vec3(flowSample2.x, 0.0f, flowSample2.y)) * 0.5f * viewValue * (clamp(fragmentDistance/5.0f, 0.0f, 1.0f) + 0.2f);
    //color = textureColor.xyz;

    vec3 reflectionColor = reflection_color(u_lights[0].position, normal);
    reflectionColor *= normalLookingUp ;//* clamp( ((fragmentDistance/8.0f)), 0.5f, 1.0f );

    vec2 bedSamplePos = vec2(fragment_position_original.x + sin(time + fragment_position_original.x)*0.1f, fragment_position_original.z + sin(time + fragment_position_original.z)*0.1f)*1.0f;
    vec3 flowSampleBed = texture( oceanFlowMap, fragment_position_original.xz * 0.05f).xyz;
    
    vec3 color = reflectionColor + waterColor; //mix(reflectionColor, waterColor, viewValue);

    vec3 diffuseColor = diffuse_color(u_lights[0].color, u_lights[0].position, normal);
    vec3 ambiantColor = ambient_color(u_lights[0].color);
    vec3 specularcolor = specular_color(u_lights[0].color, u_lights[0].position, normal);

    
    result = vec4(color * (ambiantColor + diffuseColor + specularcolor), 1.0f);

    //result = result + vec4(1.0f, 1.0f, 1.0f, 1.0f) * max((heightValue-0.95f)/0.05f * (normalLookingUp - 0.90f)/0.3f, 0.0f);

    float farFactor = (gl_FragCoord.z/gl_FragCoord.w)/100.0f;
    //result = result * (1-farFactor) + vec4(lightBlue, 1.0f) * farFactor;

    //result = viewValue * vec4(1.0f)
}

