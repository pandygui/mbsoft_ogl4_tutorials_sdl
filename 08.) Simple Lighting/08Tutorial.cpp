#include "../common_classes/common_header.h"

#include "../common_classes/SDL_OpenGLApp.h"
#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"
#include "../common_classes/vertexBufferObject.h"

#include "../common_classes/walkingCamera.h"
#include "../common_classes/static_geometry.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* One VBO, where all static data are stored now,
in this tutorial vertex is stored as 3 floats for
position and 2 floats for texture coordinate */

int iTorusFaces1, iTorusFaces2;
CVertexBufferObject vboSceneObjects;
GLuint uiVAOs[1]; // Only one VAO now

CShader shVertex, shFragment, shDirLight;
CShaderProgram spDirectionalLight;

CTexture tTextures[4];

CWalkingCamera cCamera;

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

	// Add torus to VBO and remember number of faces (triangles) of this torus
	iTorusFaces1 = GenerateTorus(vboSceneObjects, 7.0f, 2.0f, 20, 20);
	// Add sun torus to VBO
	iTorusFaces2 = GenerateTorus(vboSceneObjects, 10.0f, 6.0f, 10, 10);

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

	// Load shaders and create shader programs

	shVertex.LoadShader("data\\shaders\\tut08\\shader.vert", GL_VERTEX_SHADER);
	shFragment.LoadShader("data\\shaders\\tut08\\shader.frag", GL_FRAGMENT_SHADER);
	shDirLight.LoadShader("data\\shaders\\common\\directionalLight.frag", GL_FRAGMENT_SHADER);

	spDirectionalLight.CreateProgram();
		spDirectionalLight.AddShaderToProgram(&shVertex);
		spDirectionalLight.AddShaderToProgram(&shFragment);
		spDirectionalLight.AddShaderToProgram(&shDirLight);
	spDirectionalLight.LinkProgram();

	// Load textures

	std::string sTextureNames[] = { "ground.jpg", "box.jpg", "rust.jpg", "sun.jpg" };

	for (int i = 0; i < 4; i++)
	{
		tTextures[i].LoadTexture2D("data\\textures\\" + sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	cCamera = CWalkingCamera(glm::vec3(0.0f, 3.0f, -20.0f), glm::vec3(0.0f, 3.0f, -19.0f), glm::vec3(0.0f, 1.0f, 0.0f), 35.0f);
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

	// Set light properties

	float fSine = sin(fSunAngle*PIover180);
	glm::vec3 vSunPos(cos(fSunAngle*PIover180) * 70, sin(fSunAngle*PIover180) * 70, 0.0);

	// We'll change color of skies depending on sun's position
	glClearColor(0.0f, std::max(0.0f, 0.9f*fSine), std::max(0.0f, 0.9f*fSine), 1.0f);

	spDirectionalLight.SetUniform("sunLight.vColor", glm::vec3(1.0f, 1.0f, 1.0f));
	spDirectionalLight.SetUniform("sunLight.fAmbientIntensity", 0.25f);
	spDirectionalLight.SetUniform("sunLight.fStrength", 1.0f);
	spDirectionalLight.SetUniform("sunLight.vDirection", -glm::normalize(vSunPos));

	spDirectionalLight.SetUniform("projectionMatrix", GetProjectionMatrix());

	glm::mat4 mView = cCamera.look();
	glm::mat4 mModel(1.0);

	spDirectionalLight.SetUniform("gSampler", 0);

	spDirectionalLight.SetUniform("modelViewMatrix", mView*mModel);
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModel))));
	spDirectionalLight.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Render ground

	tTextures[0].BindTexture();
	glDrawArrays(GL_TRIANGLES, 36, 6);

	tTextures[1].BindTexture();

	for (int i = 0; i < 5; i++)
	{
		float fSign = -1.0f + float(i % 2)*2.0f; // This just returns -1.0f or 1.0f (try to examine this)
		glm::vec3 vPos = glm::vec3(fSign*15.0f, 0.0f, 50.0f - float(i)*25.0f);
		mModel = glm::translate(glm::mat4(1.0), vPos);
		mModel = glm::scale(mModel, glm::vec3(8.0f, 8.0f, 8.0f));
		// We need to trasnsform normals properly, it's done by transpose of inverse matrix of rotations and scales
		spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModel))));
		spDirectionalLight.SetUniform("modelViewMatrix", mView*mModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Render 5 tori (plural of torus :D)

	tTextures[2].BindTexture();

	for (int i = 0; i < 5; i++)
	{
		float fSign = 1.0f - float(i % 2)*2.0f; // This just returns -1.0f or 1.0f (try to examine this)
		glm::vec3 vPos = glm::vec3(fSign*15.0f, 0.0f, 50.0f - float(i)*25.0f);
		mModel = glm::translate(glm::mat4(1.0), vPos);
		mModel = glm::rotate(mModel, fGlobalAngle + i*30.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModel))));
		spDirectionalLight.SetUniform("modelViewMatrix", mView*mModel);
		glDrawArrays(GL_TRIANGLES, 42, iTorusFaces1 * 3);
	}

	tTextures[3].BindTexture();

	// Render "sun" :D

	mModel = glm::translate(glm::mat4(1.0), vSunPos);
	spDirectionalLight.SetUniform("modelViewMatrix", mView*mModel);
	spDirectionalLight.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(mModel))));

	// It must shine
	spDirectionalLight.SetUniform("sunLight.fAmbientIntensity", 0.8f);

	glDrawArrays(GL_TRIANGLES, 42 + iTorusFaces1 * 3, iTorusFaces2 * 3);

	fGlobalAngle += sof(100.0f*PIover180);
	cCamera.Update(this);

	if (Keys::Key(SDL_SCANCODE_LEFT))fSunAngle -= sof(45.0f);
	if (Keys::Key(SDL_SCANCODE_RIGHT))fSunAngle += sof(45.0f);
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{
	for (int i = 0; i < 4; i++)
		tTextures[i].DeleteTexture();

	spDirectionalLight.DeleteProgram();

	shFragment.DeleteShader();
	shVertex.DeleteShader();

	glDeleteVertexArrays(1, uiVAOs);
	vboSceneObjects.DeleteVBO();
}
