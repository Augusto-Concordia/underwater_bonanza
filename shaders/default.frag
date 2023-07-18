//default fragment shader

#version 410 core

out vec4 fragColor; //rgba color output

in vec3 vertexColor; //rgb color input from the vertex shader
in vec2 texCoord; //texture coordinate input from the vertex shader

float circle(in vec2 _st, in float _radius){
    vec2 dist = _st-vec2(0.5); //moves the circle into the center

    return 1-smoothstep(_radius-(_radius*0.01), _radius+(_radius*0.01), dot(dist, dist)*7.21); //smoothly renders a circle
}

//entrypoint
void main() {
    vec2 uv = texCoord; //get uv coordinates

    float alpha = circle(uv, 1);

    fragColor = vec4(vertexColor, alpha);
}