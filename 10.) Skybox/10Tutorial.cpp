#include "../common_classes/common_header.h"

#include "../common_classes/SDL_OpenGLApp.h"
#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"
#include "../common_classes/vertexBufferObject.h"

#include "../common_classes/flyingCamera.h"
#include "../common_classes/static_geometry.h"

#include "../common_classes/freeTypeFont_v1.h"
#include "../common_classes/skybox.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"

/* One VBO, where all static data are stored now,
in this tutorial vertex is stored as 3 floats for
position and 2 floats for texture coordinate */

int iTorusFaces;
CVertexBufferObject vboSceneObjects;
GLuint uiVAOs[1]; // Only one VAO now

CFlyingCamera cCamera;
CFreeTypeFont ftFont;

CSkybox sbMainSkybox;

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

	// Add cube to VBO

	for (int i = 0; i < 36; i++)
	{
		vboSceneObjects.AddData(&vCubeVertices[i], sizeof(glm::vec3));
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
		vboSceneObjects.AddData(&vCubeNormals[i / 6], sizeof(glm::vec3));
	}

	// Add ground to VBO

	for (int i = 0; i < 6; i++)
	{
		vboSceneObjects.AddData(&vGround[i], sizeof(glm::vec3));
		vCubeTexCoords[i] *= 10.0f;
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
		glm::vec3 vGroundNormal(0.0f, 1.0f, 0.0f);
		vboSceneObjects.AddData(&vGroundNormal, sizeof(glm::vec3));
	}

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
	glClearColor(0.0f, 0.26f, 0.48f, 1.0f);

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	ftFont.LoadSystemFont("arial.ttf", 32);
	ftFont.SetShaderProgram(&spFont2D);

	cCamera = CFlyingCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.SetMovingKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);

	sbMainSkybox.LoadSkybox("data\\skyboxes\\jajlands1\\", "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg");
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

	spDirectionalLight.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.SetUniform("sunLight.fAmbientIntensity", 1.0f); // Full light for skybox
	spDirectionalLight.SetUniform("sunLight.vDirection", glm::vec3(0, -1, 0));

	spDirectionalLight.SetUniform("projectionMatrix", GetProjectionMatrix());
	spDirectionalLight.SetUniform("gSampler", 0);

	glm::mat4 mModelView = cCamera.Look();
	glm::mat4 mModelToCamera(1.0);

	spDirectionalLight.SetUniform("modelViewMatrix", glm::translate(mModelView, cCamera.GetEye()));
	sbMainSkybox.RenderSkybox();

	glBindVertexArray(uiVAOs[0]);

	// After skybox is rendered, set light to some reasonable values

	glm::vec3 vSunPos(cos(fSunAngle*PIover180) * 70, sin(fSunAngle*PIover180) * 70, 0.0);

	spDirectionalLight.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.SetUniform("sunLight.vDirection", -glm::normalize(vSunPos));
	spDirectionalLight.SetUniform("sunLight.fAmbientIntensity", 0.55f);

	// Render ground

	spDirectionalLight.SetUniform("modelViewMatrix", &mModelView);
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
	spDirectionalLight.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	tTextures[0].BindTexture();
	glDrawArrays(GL_TRIANGLES, 36, 6);

	tTextures[1].BindTexture();

	for (int nb = 1; nb <= 9; nb++)
	{
		int iCnt = nb > 5 ? 10 - nb : nb;
		for (int i = 0; i < iCnt; i++)
		{
			glm::vec3 vPos = glm::vec3(-20.0f + nb*8.02f, -6.0f + i*8.02f, -50.0f);
			mModelToCamera = glm::translate(glm::mat4(1.0), vPos);
			mModelToCamera = glm::scale(mModelToCamera, glm::vec3(8.0f, 8.0f, 8.0f));
			// We need to transform normals properly, it's done by transpose of inverse matrix of rotations and scales
			spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
			spDirectionalLight.SetUniform("modelViewMatrix", mModelView*mModelToCamera);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// Render 3 rotated tori to create interesting object

	tTextures[2].BindTexture();

	glm::vec3 vPos = glm::vec3(0.0f, 0.0, 40.0f);
	mModelToCamera = glm::translate(glm::mat4(1.0), vPos);
	mModelToCamera = glm::rotate(mModelToCamera, fGlobalAngle*PIover180, glm::vec3(0.0f, 1.0f, 0.0f));
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
	spDirectionalLight.SetUniform("modelViewMatrix", mModelView*mModelToCamera);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces * 3);

	mModelToCamera = glm::translate(glm::mat4(1.0), vPos);
	mModelToCamera = glm::rotate(mModelToCamera, fGlobalAngle*PIover180, glm::vec3(0.0f, 1.0f, 0.0f));
	mModelToCamera = glm::rotate(mModelToCamera, 90.0f*PIover180, glm::vec3(1.0f, 0.0f, 0.0f));
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
	spDirectionalLight.SetUniform("modelViewMatrix", mModelView*mModelToCamera);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces * 3);

	mModelToCamera = glm::translate(glm::mat4(1.0), vPos);
	mModelToCamera = glm::rotate(mModelToCamera, (fGlobalAngle + 90.0f)*PIover180, glm::vec3(0.0f, 1.0f, 0.0f));
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModelToCamera))));
	spDirectionalLight.SetUniform("modelViewMatrix", mModelView*mModelToCamera);
	glDrawArrays(GL_TRIANGLES, 42, iTorusFaces * 3);

	fGlobalAngle += sof(100.0f);
	cCamera.Update(this);

	// Print something over scene

	spFont2D.UseProgram();
	glDisable(GL_DEPTH_TEST);
	spFont2D.SetUniform("projectionMatrix", GetOrthoMatrix());
	spFont2D.SetUniform("vColor", glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

	ftFont.Print("www.mbsoftworks.sk", 20, 20);

	glEnable(GL_DEPTH_TEST);
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
	spOrtho2D.DeleteProgram();

	for (int i = 0; i < NUMSHADERS; i++)
		shShaders[i].DeleteShader();

	glDeleteVertexArrays(1, uiVAOs);
	vboSceneObjects.DeleteVBO();

	sbMainSkybox.DeleteSkybox();
}
