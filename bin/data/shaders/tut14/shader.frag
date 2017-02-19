#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec3 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

#include "../common/directionalLight.frag"

uniform DirectionalLight sunLight;

void main()
{
	vec3 vNormalized = normalize(vNormal);
	
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	vec4 vDirLightColor = getDirectionalLightColor(sunLight, vNormal);
	
	outputColor = vTexColor*vColor*vDirLightColor;
}