#version 330

#include_part

struct PointLight
{
	vec3 vColor; // Color of the point light
	vec3 vPosition; // And its position
	
	// Attenuation factor - constant, linear and exponential
	float fConstantAtt;
	float fLinearAtt;
	float fExpAtt;
};

vec4 getPointLightColor(const PointLight ptLight, vec3 vWorldPos, vec3 vNormal);

#definition_part

vec4 getPointLightColor(const PointLight ptLight, vec3 vWorldPos, vec3 vNormal)
{
	vec3 vPosToLight = vWorldPos-ptLight.vPosition;
	float fDist = length(vPosToLight);
	vPosToLight = normalize(vPosToLight);
	
	float fDiffuseFactor = max(0.0, dot(vNormal, -vPosToLight));
	float fAttenuationTotal = ptLight.fConstantAtt + ptLight.fLinearAtt*fDist + ptLight.fExpAtt*fDist*fDist;
	float fFinalFactor = clamp(fDiffuseFactor/fAttenuationTotal, 0.0, 1.0);

	return vec4(ptLight.vColor, 1.0)*fFinalFactor;
}