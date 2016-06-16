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
	gl_Position = projectionMatrix*modelViewMatrix*vec4(inPosition, 1.0);
	texCoord = inCoord;
	vec3 vTransformedNormal = normalMatrix*inNormal;
	vNormal = vTransformedNormal;
}