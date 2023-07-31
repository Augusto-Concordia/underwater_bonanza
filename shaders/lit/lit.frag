//default lit fragment shader

#version 330 core

uniform vec3 u_cam_pos; //cam position

uniform vec3 u_light_pos; //main light position
uniform vec3 u_light_color; //main light color

uniform float u_ambient_strength; //ambient light strength
uniform float u_specular_strength; //specular light strength
uniform int u_shininess; //light shininess

uniform vec3 u_color; //cube color
uniform float u_alpha; //cube opacity

uniform sampler2D u_depth_texture; //light screen depth texture

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

layout(location = 0) out vec4 out_color; //rgba color output
//layout(location = 1) out vec4 depth_color; //depth color output

//entrypoint
void main() {
    float light_strength = 20;

    //ambient lighting calculation
    vec3 ambient = u_ambient_strength * u_light_color;

    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_light_pos - FragPos);
    float lightDistance = length(u_light_pos - FragPos);

    float diffFactor = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diffFactor * u_light_color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, norm));

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * u_specular_strength * u_light_color;

    //shadow calculation
    vec3 projectedCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projectedCoords = projectedCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range LightSpaceFragPos as coords)
    float closestDepth = texture(u_depth_texture, projectedCoords.xy).r;

    // get current linear depth as stored in the depth buffer
    float currentDepth = projectedCoords.z;

    float shadowScalar = (currentDepth - 0.003) < closestDepth ? 1.0 : 0.0;

    vec3 colorResult = u_color * (ambient + (diffuse + specular) * shadowScalar * light_strength * 0.883 / (0.18 + 0.0 * lightDistance + 0.51 * lightDistance * lightDistance));

    out_color = vec4(colorResult, u_alpha);
    gl_FragDepth = gl_FragCoord.z;
    //depth_color = vec4(vec3(length(u_light_pos - FragPos)/100.0), 1.0);
}