#version 330

uniform struct Matrices
{
	mat4 projectionMatrix;
	mat4 modelViewMatrix;
	mat3 normalMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec2 texCoord;
smooth out vec3 vNormal;
smooth out vec4 vEyeSpacePos;

void main()
{
	vec4 vEyeSpacePosVertex = matrices.modelViewMatrix*vec4(inPosition, 1.0);
	gl_Position = matrices.projectionMatrix*vEyeSpacePosVertex;
	texCoord = inCoord;
	vNormal = matrices.normalMatrix*inNormal;
	
	vEyeSpacePos = vEyeSpacePosVertex;	
}