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
	vec4 vDirectionalLightColor = getDirectionalLightColor(sunLight, vNormal);
	outputColor = vTexColor*vColor*vDirectionalLightColor;
}