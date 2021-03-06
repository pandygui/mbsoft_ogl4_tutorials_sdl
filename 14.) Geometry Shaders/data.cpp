#include "../common_classes/common_header.h"

#include "data.h"

CShader shShaders[NUMSHADERS];
CShaderProgram spMain, spFont2D;

CTexture tTextures[NUMTEXTURES];

//--------------------------------------------------------------------
/// \brief Loads all shaders in this tutorial
//--------------------------------------------------------------------

void LoadShaders()
{
	// Load shaders and create shader programs

	shShaders[0].LoadShader("data\\shaders\\tut14\\shader.vert", GL_VERTEX_SHADER);
	shShaders[1].LoadShader("data\\shaders\\tut14\\shader.geom", GL_GEOMETRY_SHADER);
	shShaders[2].LoadShader("data\\shaders\\tut14\\shader.frag", GL_FRAGMENT_SHADER);

	shShaders[3].LoadShader("data\\shaders\\tut09\\ortho2D.vert", GL_VERTEX_SHADER);
	shShaders[4].LoadShader("data\\shaders\\tut09\\font2D.frag", GL_FRAGMENT_SHADER);
	shShaders[5].LoadShader("data\\shaders\\common\\directionalLight.frag", GL_FRAGMENT_SHADER);

	spMain.CreateProgram();
		spMain.AddShaderToProgram(&shShaders[0]);
		spMain.AddShaderToProgram(&shShaders[1]);
		spMain.AddShaderToProgram(&shShaders[2]);
		spMain.AddShaderToProgram(&shShaders[5]);
	spMain.LinkProgram();

	spFont2D.CreateProgram();
		spFont2D.AddShaderToProgram(&shShaders[3]);
		spFont2D.AddShaderToProgram(&shShaders[4]);
	spFont2D.LinkProgram();
}

//--------------------------------------------------------------------
/// \brief Loads all textures in this tutorial
//--------------------------------------------------------------------

void LoadTextures()
{
	std::string sTextureNames[] = { "Tile41a.jpg", "crate.jpg", "metalplate.jpg", "stone_wall.jpg" };

	for (int i = 0; i < NUMTEXTURES; i++)
	{
		tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
}