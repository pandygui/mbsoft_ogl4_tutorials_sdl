#version 330

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

out vec2 texCoord;

smooth out vec3 vNormal;

void main()
{
	vec4 vEyeSpacePos = modelViewMatrix*vec4(inPosition, 1.0);
	gl_Position = projectionMatrix*vEyeSpacePos;
	texCoord = inCoord;
	vNormal = normalMatrix*inNormal;
}