#include "../common_classes/common_header.h"

#include "data.h"

CShader shShaders[NUMSHADERS];
CShaderProgram spPointLights, spOrtho2D, spFont2D, spColor;

CTexture tTextures[NUMTEXTURES];

//--------------------------------------------------------------------
/// \brief Loads all shaders in this tutorial
//--------------------------------------------------------------------

void LoadShaders()
{
	// Load shaders and create shader programs

	shShaders[0].LoadShader("data\\shaders\\tut13\\shader.vert", GL_VERTEX_SHADER);
	shShaders[1].LoadShader("data\\shaders\\tut13\\shader.frag", GL_FRAGMENT_SHADER);
	shShaders[2].LoadShader("data\\shaders\\tut09\\ortho2D.vert", GL_VERTEX_SHADER);
	shShaders[3].LoadShader("data\\shaders\\tut09\\ortho2D.frag", GL_FRAGMENT_SHADER);
	shShaders[4].LoadShader("data\\shaders\\tut09\\font2D.frag", GL_FRAGMENT_SHADER);
	shShaders[5].LoadShader("data\\shaders\\common\\directionalLight.frag", GL_FRAGMENT_SHADER);
	shShaders[6].LoadShader("data\\shaders\\common\\pointLight.frag", GL_FRAGMENT_SHADER);

	shShaders[7].LoadShader("data\\shaders\\common\\simpleColor.vert", GL_VERTEX_SHADER);
	shShaders[8].LoadShader("data\\shaders\\common\\simpleColor.frag", GL_FRAGMENT_SHADER);

	spPointLights.CreateProgram();
		spPointLights.AddShaderToProgram(&shShaders[0]);
		spPointLights.AddShaderToProgram(&shShaders[1]);
		spPointLights.AddShaderToProgram(&shShaders[5]);
		spPointLights.AddShaderToProgram(&shShaders[6]);
	spPointLights.LinkProgram();

	spOrtho2D.CreateProgram();
		spOrtho2D.AddShaderToProgram(&shShaders[2]);
		spOrtho2D.AddShaderToProgram(&shShaders[3]);
	spOrtho2D.LinkProgram();

	spFont2D.CreateProgram();
		spFont2D.AddShaderToProgram(&shShaders[2]);
		spFont2D.AddShaderToProgram(&shShaders[4]);
	spFont2D.LinkProgram();

	spColor.CreateProgram();
		spColor.AddShaderToProgram(&shShaders[7]);
		spColor.AddShaderToProgram(&shShaders[8]);
	spColor.LinkProgram();
}

//--------------------------------------------------------------------
/// \brief Loads all textures in this tutorial
//--------------------------------------------------------------------

void LoadTextures()
{
	std::string sTextureNames[] = { "grass.jpg", "crate.jpg", "metalplate.jpg", "stone_wall.jpg" };

	for (int i = 0; i < NUMTEXTURES; i++)
	{
		tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
}