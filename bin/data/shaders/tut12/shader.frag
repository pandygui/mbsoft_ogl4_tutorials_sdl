#version 330

smooth in vec2 texCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

uniform struct SimpleDirectionalLight
{
	vec3 vColor;
	vec3 vDirection;
	float fAmbientIntensity;
} sunLight;

uniform struct FogParameters
{
	vec4 vFogColor; // Fog color
	float fStart; // This is only for linear fog
	float fEnd; // This is only for linear fog
	float fDensity; // For exp and exp2 equation
	
	int iEquation; // 0 = linear, 1 = exp, 2 = exp2
} fogParams;

float getFogFactor(FogParameters params, float fFogCoord);

void main()
{
	vec4 vTexColor = texture2D(gSampler, texCoord);
	float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection));
	outputColor = vTexColor*vColor*vec4(sunLight.vColor*(sunLight.fAmbientIntensity+fDiffuseIntensity), 1.0);
	
	// Add fog
	float fFogCoord = abs(vEyeSpacePos.z/vEyeSpacePos.w);
	outputColor = mix(outputColor, fogParams.vFogColor, getFogFactor(fogParams, fFogCoord));
}