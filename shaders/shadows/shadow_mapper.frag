//default shadow mapper fragment shader

#version 330 core

in vec3 FragPos;

layout(location = 0) out vec3 out_color; //rgba color output

//entrypoint
void main() {
    //eliminates the harsh edge of the shadow map, comes from: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
    //gl_FragDepth = (1.0 - FragPos.z / 400.0) < 0.0 ? 0.0 : (1.0 - FragPos.z / 400.0);

    //out_color = vec3(1.0);//vec3(map(FragPos.z, 0.1, 400.0, 0.0, 255.0));
}