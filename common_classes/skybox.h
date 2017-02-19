#pragma once

#include "texture.h"
#include "vertexBufferObject.h"

/********************************

Class:		CSkybox

Purpose:	Class for using skybox.

********************************/

class CSkybox
{
public:
	void LoadSkybox(std::string a_sDirectory, std::string a_sFront, std::string a_sBack, std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom);
	void RenderSkybox();

	void DeleteSkybox();
private:
	GLuint uiVAO;
	CVertexBufferObject vboRenderData;
	CTexture tTextures[6];
	std::string sDirectory;
	std::string sFront, sBack, sLeft, sRight, sTop, sBottom;
};