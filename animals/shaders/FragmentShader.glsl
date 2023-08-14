#version 420 core

in vec3 vertexColor;
in vec3 vertexPosition;
in vec2 vertexUV;
out vec4 FragColor;
uniform vec3 lightPos;
uniform sampler2D textureSampler;
in vec3 pos;
in vec3 normal;
in vec3 eyeDir;
in vec3 lightDir;


void main()
{
                vec3 lightColor = vec3(1.0f,1.0f,1.0f);
                 float lightPower = 30.0f;
                vec3 diffuseColor = vertexColor;
                vec3 specularColor = vec3(0.5f,0.5f,0.5f);
                vec3 ambientColor = vec3(0.9f,0.9f,0.9f) * vertexColor;
                float dist = length(lightPos - pos);
                vec3 n = normalize(normal);
                vec3 l = normalize(lightDir);
                float cosTheta = clamp( dot(n,l), 0, 1);
                vec3 E = normalize(eyeDir);
                vec3 R = reflect(-l, n);
                float cosAlpha = clamp(dot(E, R), 0, 1);
               vec4 textureColor = texture( textureSampler, vertexUV );
               diffuseColor *= lightPower * cosTheta / (1 + dist * 0.5 + dist * dist * 0.05) ;
               specularColor *= lightColor * lightPower * pow(cosAlpha,5) / (1 + dist * 0.5 + dist * dist * 0.05);
	FragColor = vec4(ambientColor+diffuseColor+specularColor,1.0f);
    


}