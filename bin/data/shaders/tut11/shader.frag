#version 330

in vec2 texCoord;
smooth in vec3 vNormal;
out vec4 outputColor;

uniform sampler2D gSamplers[2];
uniform float fTextureContributions[2];
uniform vec4 vColor;
uniform int numTextures;

struct SimpleDirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbientIntensity;
};

uniform SimpleDirectionalLight sunLight;

void main()
{
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
	float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection));
	float fTotalSunlightIntensity = clamp(sunLight.fAmbientIntensity+fDiffuseIntensity, 0.0, 1.0);
	outputColor *= vColor*vec4(sunLight.vColor*fTotalSunlightIntensity, 1.0);
}