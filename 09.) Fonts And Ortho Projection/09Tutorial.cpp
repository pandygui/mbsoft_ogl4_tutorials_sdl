#include "../common_classes/common_header.h"

#include "../common_classes/SDL_OpenGLApp.h"
#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"
#include "../common_classes/vertexBufferObject.h"

#include "../common_classes/walkingCamera.h"
#include "../common_classes/static_geometry.h"

#include "../common_classes/freeTypeFont_v1.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"

/* One VBO, where all static data are stored now,
in this tutorial vertex is stored as 3 floats for
position and 2 floats for texture coordinate */

int iTorusFaces;
CVertexBufferObject vboSceneObjects;
GLuint uiVAOs[1]; // Only one VAO now

CWalkingCamera cCamera;
CFreeTypeFont ftFont;
int iCurrentFontSize = 24;

//--------------------------------------------------------------------
/// \brief Initializes scene before rendering
//--------------------------------------------------------------------

void CSDLOpenGLWindow::InitScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	vboSceneObjects.CreateVBO();
	glGenVertexArrays(1, uiVAOs); // Create one VAO
	glBindVertexArray(uiVAOs[0]);

	vboSceneObjects.BindVBO();
	iTorusFaces = GenerateTorus(vboSceneObjects, 7.0f, 2.0f, 20, 20);
	vboSceneObjects.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	LoadShaders();
	LoadTextures();

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	ftFont.LoadSystemFont("comic.ttf", 32);
	ftFont.SetShaderProgram(&spFont2D);

	cCamera = CWalkingCamera(glm::vec3(0.0f, 1.0f, -30.0f), glm::vec3(0.0f, 1.0f, -29.0f), glm::vec3(0.0f, 1.0f, 0.0f), 35.0f);
}

//--------------------------------------------------------------------
/// \brief  Renders whole scene
//--------------------------------------------------------------------

const float PIover180 = 3.1415f / 180.0f;
float fGlobalAngle;
float fSunAngle = 45.0f;

void CSDLOpenGLWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	spDirectionalLight.UseProgram();
	glBindVertexArray(uiVAOs[0]);

	// Set some lighting parameters

	spDirectionalLight.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.SetUniform("sunLight.fAmbientIntensity", 0.25f);
	spDirectionalLight.SetUniform("sunLight.vDirection", glm::vec3(0, -1, 0));
	spDirectionalLight.SetUniform("sunLight.fStrength", 1.0f);

	spDirectionalLight.SetUniform("projectionMatrix", GetProjectionMatrix());

	glm::mat4 mModelView = cCamera.look();
	glm::mat4 mModelToCamera = glm::rotate(glm::mat4(1.0f), fGlobalAngle*PIover180, glm::vec3(0.0f, 1.0f, 0.0f));

	// Render torus

	spDirectionalLight.SetUniform("gSampler", 0);
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
	spDirectionalLight.SetUniform("modelViewMatrix", mModelView*mModelToCamera);
	spDirectionalLight.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	tTextures[0].BindTexture();
	glDrawArrays(GL_TRIANGLES, 0, iTorusFaces * 3);

	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	spFont2D.UseProgram();
	// Font color, you can even change transparency of font with alpha parameter
	spFont2D.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spFont2D.SetUniform("projectionMatrix", GetOrthoMatrix());

	char buf[255];
	sprintf(buf, "Font Size: %dpx\nPress UP and DOWN arrow key to change\n\nTotal Torus Faces: %d", iCurrentFontSize, iTorusFaces);

	ftFont.Print(buf, 20, GetViewportHeight() - 10 - iCurrentFontSize, iCurrentFontSize);

	ftFont.Print("www.mbsoftworks.sk", 20, 20, 24);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	fGlobalAngle += sof(100.0f);
	cCamera.Update(this);

	if (Keys::Onekey(SDL_SCANCODE_UP))
		iCurrentFontSize++;

	if (Keys::Onekey(SDL_SCANCODE_DOWN))iCurrentFontSize--;
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{
	for (int i = 0; i < NUMTEXTURES; i++)
		tTextures[i].DeleteTexture();

	spDirectionalLight.DeleteProgram();
	spFont2D.DeleteProgram();

	for (int i = 0; i < NUMSHADERS; i++)
		shShaders[i].DeleteShader();

	glDeleteVertexArrays(1, uiVAOs);
	vboSceneObjects.DeleteVBO();
}
