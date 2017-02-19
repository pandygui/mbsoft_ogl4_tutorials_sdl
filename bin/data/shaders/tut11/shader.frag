#version 330

in vec2 texCoord;
smooth in vec3 vNormal;
out vec4 outputColor;

uniform sampler2D gSamplers[2];
uniform float fTextureContributions[2];
uniform vec4 vColor;
uniform int numTextures;

#include "../common/directionalLight.frag"

uniform DirectionalLight sunLight;

void main()
{
	vec3 vNormalized = normalize(vNormal);
	outputColor = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i = 0; i < numTextures; i++)
	{
		if(i == 0)
		{
			vec4 vTexColor = texture2D(gSamplers[0], texCoord);
			outputColor += vTexColor*fTextureContributions[0];
		}
		else
		{
				vec4 vTexColor = texture2D(gSamplers[1], texCoord);
				outputColor += vTexColor*fTextureContributions[1];
		}
	}
	vec4 vDirectionalLightColor = getDirectionalLightColor(sunLight, vNormalized);
	outputColor *= vColor*vDirectionalLightColor;
}