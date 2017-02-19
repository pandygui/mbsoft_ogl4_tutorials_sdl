#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 9)out;

uniform struct Matrices
{
	mat4 projectionMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;                                                                           
	mat3 normalMatrix;
} matrices;

in vec2 vTexCoordPass[];
in vec3 vNormalPass[];
vec3 vNormalTransformed[3];

smooth out vec3 vEyeSpacePos;
smooth out vec3 vNormal;
smooth out vec3 vWorldPos;
smooth out vec2 vTexCoord;

uniform float fBender;

void main()
{
	mat4 mMVP = matrices.projectionMatrix*matrices.viewMatrix*matrices.modelMatrix;
	
	for(int i = 0; i < 3; i++)
		vNormalTransformed[i] = vNormalPass[i]*matrices.normalMatrix;

	vec3 vNormalized = normalize(vNormalTransformed[0]+vNormalTransformed[1]+vNormalTransformed[2])/3.0;
	vec3 vMiddle = (gl_in[0].gl_Position.xyz+gl_in[1].gl_Position.xyz+gl_in[2].gl_Position.xyz)/3.0+vNormalized*fBender;
	vec2 vTexCoordMiddle = (vTexCoordPass[0]+vTexCoordPass[1]+vTexCoordPass[2])/3.0;

	for(int i = 0; i < 3; i++)
	{
	  vec3 vPos = gl_in[i].gl_Position.xyz;
	  gl_Position = mMVP*vec4(vPos, 1.0);
	  vNormal = vNormalTransformed[i];
	  vTexCoord = vTexCoordPass[i];
	  EmitVertex();

	  vPos = gl_in[(i+1)%3].gl_Position.xyz;
	  gl_Position = mMVP*vec4(vPos, 1.0);
	  vNormal = vNormalTransformed[i];
	  vTexCoord = vTexCoordPass[(i+1)%3];
	  EmitVertex();

	  gl_Position = mMVP*vec4(vMiddle, 1.0);
	  vNormal = vNormalTransformed[i];
	  vTexCoord = vTexCoordMiddle;
	  EmitVertex();

	  EndPrimitive();
	}
}