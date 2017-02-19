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
		glm::vec2 vCoord = vCubeTexCoords[i];
		vCoord *= 50.0f;
		vboSceneObjects.AddData(&vCoord, sizeof(glm::vec2));
		glm::vec3 vGroundNormal(0.0f, 1.0f, 0.0f);
		vboSceneObjects.AddData(&vGroundNormal, sizeof(glm::vec3));
	}

	// Add building to VBO

	for (int i = 0; i < 24; i++)
	{
		vboSceneObjects.AddData(&vBuilding[i], sizeof(glm::vec3));
		glm::vec2 vCoord = vCubeTexCoords[i % 6] * 7.0f;
		vboSceneObjects.AddData(&vCoord, sizeof(glm::vec2));
		vboSceneObjects.AddData(&vBuildingNormals[i / 6], sizeof(glm::vec3));
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

	LoadShaders(); // Load shaders and create shader programs
	LoadTextures(); // Load textures

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glClearColor(0.0f, 0.26f, 0.48f, 1.0f);

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	ftFont.LoadSystemFont("arial.ttf", 32);
	ftFont.SetShaderProgram(&spFont2D);

	cCamera = CFlyingCamera(glm::vec3(0.0f, 10.0f, 120.0f), glm::vec3(0.0f, 10.0f, 119.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.SetMovingKeys(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);

	sbMainSkybox.LoadSkybox("data\\skyboxes\\jajsundown1\\", "jajsundown1_ft.jpg", "jajsundown1_bk.jpg", "jajsundown1_lf.jpg", "jajsundown1_rt.jpg", "jajsundown1_up.jpg", "jajsundown1_dn.jpg");
}

//--------------------------------------------------------------------
/// \brief  Renders whole scene
//--------------------------------------------------------------------

const float PIover180 = 3.1415f / 180.0f;
float fGlobalAngle;
float fBender = 0.75f;
bool bWireFrame = false;

void CSDLOpenGLWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	spMain.UseProgram();
	spMain.SetUniform("fBender", fBender);

	spMain.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
	spMain.SetUniform("sunLight.fAmbientIntensity", 1.0f);
	spMain.SetUniform("sunLight.fStrength", 0.0f);
	spMain.SetUniform("sunLight.vDirection", glm::vec3(0, -1, 0));

	spMain.SetUniform("matrices.projectionMatrix", GetProjectionMatrix());
	spMain.SetUniform("gSampler", 0);

	glm::mat4 mView = cCamera.Look();
	glm::mat4 mModelMatrix;

	spMain.SetUniform("matrices.viewMatrix", &mView);
	mModelMatrix = glm::translate(glm::mat4(1.0f), cCamera.GetEye());

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModelMatrix);
	sbMainSkybox.RenderSkybox();

	glBindVertexArray(uiVAOs[0]);
	spMain.SetUniform("sunLight.fAmbientIntensity", 0.5f);
	spMain.SetUniform("sunLight.fStrength", 1.0f);

	// Render ground

	if (bWireFrame)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::mat4(1.0f));

	spMain.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	tTextures[0].BindTexture();
	glDrawArrays(GL_TRIANGLES, 36, 6);

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -10.0f, 0.0f)));

	tTextures[3].BindTexture();
	glDrawArrays(GL_TRIANGLES, 42, 24);

	// Create a box pile inside "building"

	tTextures[1].BindTexture();

	for (int numBoxes = 1; numBoxes <= 9; numBoxes++)
	{
		int iCnt = numBoxes > 5 ? 10 - numBoxes : numBoxes;
		for (int i = 0; i < iCnt; i++)
		{
			glm::vec3 vPos = glm::vec3(-40.0f + numBoxes*8.02f, -10.0f + 4.0f + i*8.02f, 0.0f);
			mModelMatrix = glm::translate(glm::mat4(1.0), vPos);
			mModelMatrix = glm::scale(mModelMatrix, glm::vec3(8.0f, 8.0f, 8.0f));
			// We need to transform normals properly, it's done by transpose of inverse matrix of rotations and scales
			spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModelMatrix);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	// Render 3 rotated tori to create interesting object

	tTextures[2].BindTexture();

	// Now it's gonna float in the air
	glm::vec3 vPos = glm::vec3(0.0f, 50.0, 0.0f);
	mModelMatrix = glm::translate(glm::mat4(1.0), vPos);
	mModelMatrix = glm::rotate(mModelMatrix, fGlobalAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 66, iTorusFaces * 3);

	mModelMatrix = glm::translate(glm::mat4(1.0), vPos);
	mModelMatrix = glm::rotate(mModelMatrix, fGlobalAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	mModelMatrix = glm::rotate(mModelMatrix, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 66, iTorusFaces * 3);

	mModelMatrix = glm::translate(glm::mat4(1.0), vPos);
	mModelMatrix = glm::rotate(mModelMatrix, fGlobalAngle + 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModelMatrix);
	glDrawArrays(GL_TRIANGLES, 66, iTorusFaces * 3);

	fGlobalAngle += sof(100.0f*PIover180);
	cCamera.Update(this);

	// Print something over scene

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	spFont2D.UseProgram();
	glDisable(GL_DEPTH_TEST);
	spFont2D.SetUniform("projectionMatrix", GetOrthoMatrix());

	int w = GetViewportWidth(), h = GetViewportHeight();

	spFont2D.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ftFont.Print("www.mbsoftworks.sk", 20, 20, 24);

	ftFont.PrintFormatted(20, h - 30, 20, "Bender Value: %.3f  (Hold Z and X to change)", fBender);
	ftFont.PrintFormatted(20, h - 55, 20, "Polygon Mode: %s (Press Q to toggle)", bWireFrame ? "Wireframe" : "Normal");

	if (Keys::Onekey(SDL_SCANCODE_Q))
		bWireFrame = !bWireFrame;
	
	if (Keys::Key(SDL_SCANCODE_X))fBender += sof(2.0f);
	if (Keys::Key(SDL_SCANCODE_Z))fBender -= sof(2.0f);

	glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{
	for (int i = 0; i < NUMTEXTURES; i++)
		tTextures[i].DeleteTexture();

	spMain.DeleteProgram();
	spFont2D.DeleteProgram();

	for (int i = 0; i < NUMSHADERS; i++)
		shShaders[i].DeleteShader();

	glDeleteVertexArrays(1, uiVAOs);
	vboSceneObjects.DeleteVBO();

	sbMainSkybox.DeleteSkybox();
}
