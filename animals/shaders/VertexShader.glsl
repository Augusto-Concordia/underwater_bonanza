#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 worldMatrix;
uniform mat4 worldOrientationMatrix = mat4(1.0);
uniform mat4 viewMatrix = mat4(1.0);
uniform mat4 projectionMatrix = mat4(1.0);
uniform vec3 customColor = vec3(1.0);
uniform vec3 lightPos;

out vec3 vertexColor;
out vec3 vertexPosition;
out vec3 pos;
out vec3 normal;
out vec3 eyeDir;
out vec3 lightDir;
out vec2 vertexUV;

void main()
{
	vertexColor = customColor;
	vertexPosition = aPos;
	mat4 modelViewProjection = projectionMatrix * viewMatrix * worldOrientationMatrix * worldMatrix;
	gl_Position = modelViewProjection * vec4(aPos, 1.0);
	pos = (worldMatrix * vec4(aPos, 1)).xyz;
	vec3 vertexPosition_cameraspace = ( viewMatrix * worldMatrix * vec4(aPos,1)).xyz;
	eyeDir = vec3(0,0,0) - vertexPosition_cameraspace;
	vec3 lightPosCamera = (viewMatrix * vec4(lightPos, 1)).xyz;
	lightDir = lightPosCamera + eyeDir;
	normal = (transpose(inverse(viewMatrix * worldMatrix)) * vec4(aNormal, 1)).xyz;
	vertexUV = aUV;
}