#include "../common_classes/common_header.h"

#include "data.h"

CShader shShaders[NUMSHADERS];
CShaderProgram spDirectionalLight, spOrtho2D, spFont2D;

CTexture tTextures[NUMTEXTURES];

//--------------------------------------------------------------------
/// \brief Loads all shaders in this tutorial
//--------------------------------------------------------------------

void LoadShaders()
{
	// Load shaders and create shader programs

	shShaders[0].LoadShader("data\\shaders\\tut09\\shader.vert", GL_VERTEX_SHADER);
	shShaders[1].LoadShader("data\\shaders\\tut09\\shader.frag", GL_FRAGMENT_SHADER);
	shShaders[2].LoadShader("data\\shaders\\tut09\\ortho2D.vert", GL_VERTEX_SHADER);
	shShaders[3].LoadShader("data\\shaders\\tut09\\ortho2D.frag", GL_FRAGMENT_SHADER);
	shShaders[4].LoadShader("data\\shaders\\tut09\\font2D.frag", GL_FRAGMENT_SHADER);

	spDirectionalLight.CreateProgram();
		spDirectionalLight.AddShaderToProgram(&shShaders[0]);
		spDirectionalLight.AddShaderToProgram(&shShaders[1]);
	spDirectionalLight.LinkProgram();

	spOrtho2D.CreateProgram();
		spOrtho2D.AddShaderToProgram(&shShaders[2]);
		spOrtho2D.AddShaderToProgram(&shShaders[3]);
	spOrtho2D.LinkProgram();

	spFont2D.CreateProgram();
		spFont2D.AddShaderToProgram(&shShaders[2]);
		spFont2D.AddShaderToProgram(&shShaders[4]);
	spFont2D.LinkProgram();
}

//--------------------------------------------------------------------
/// \brief Loads all textures in this tutorial
//--------------------------------------------------------------------

void LoadTextures()
{
	string sTextureNames[] = { "rust.jpg" };

	for (int i = 0; i < NUMTEXTURES; i++) // I know that FOR cycle is useless now, but it was easier to rewrite :)
	{
		tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
}