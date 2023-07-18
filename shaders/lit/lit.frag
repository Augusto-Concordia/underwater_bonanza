//default lit fragment shader

#version 410 core

uniform vec3 u_cam_pos; //cam position

uniform vec3 u_light_pos; //main light position
uniform vec3 u_light_color; //main light color

uniform float u_ambient_strength; //ambient light strength
uniform float u_specular_strength; //specular light strength
uniform int u_shininess; //light shininess

uniform vec3 u_color; //cube color
uniform float u_alpha; //cube opacity

in vec3 FragPos;
in vec3 Normal;

out vec4 fragColor; //rgba color output

//entrypoint
void main() {
    //ambient lighting calculation
    vec3 ambient = u_ambient_strength * u_light_color;

    //diffuse lighting calculation
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_light_pos - FragPos);

    float diffFactor = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffFactor * u_light_color;

    //specular lighting calculation
    vec3 viewDir = normalize(u_cam_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), u_shininess);
    vec3 specular = specularFactor * u_specular_strength * u_light_color;

    vec3 colorResult = (ambient + diffuse + specular) * u_color;

    fragColor = vec4(colorResult, u_alpha);
}