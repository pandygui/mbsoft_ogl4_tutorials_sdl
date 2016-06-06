#version 330

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2D gSampler;

void main()
{
	outputColor = texture(gSampler, vTexCoord);
}