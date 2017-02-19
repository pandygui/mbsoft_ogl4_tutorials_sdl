#version 330

#include_part

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbientIntensity;
	// After calculation of diffuse intensity, the number is scaled by fStrength
	float fStrength;
};

vec4 getDirectionalLightColor(DirectionalLight dirLight, vec3 vNormal);

#definition_part

vec4 getDirectionalLightColor(DirectionalLight dirLight, vec3 vNormal)
{
	float fDiffuseIntensity = max(0.0, dot(vNormal, -dirLight.vDirection));
	float fFinalIntensity = clamp(dirLight.fAmbientIntensity+fDiffuseIntensity*dirLight.fStrength, 0.0, 1.0);
	return vec4(dirLight.vColor*fFinalIntensity, 1.0);
}