#version 330

in vec2 texCoord;
smooth in vec3 vNormal;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

#include "../common/directionalLight.frag"

uniform DirectionalLight sunLight;

void main()
{
	vec4 vTexColor = texture2D(gSampler, texCoord);
	float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection));
	outputColor = vTexColor*vColor*vec4(sunLight.vColor*(sunLight.fAmbientIntensity+fDiffuseIntensity), 1.0);
}