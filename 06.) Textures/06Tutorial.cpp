#include "../common_classes/common_header.h"

#include "../common_classes/SDL_OpenGLApp.h"
#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"
#include "../common_classes/vertexBufferObject.h"

#include "../common_classes/static_geometry.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

CVertexBufferObject vboSceneObjects;
GLuint uiVAO; // And one VAO

CShader shVertex, shFragment;
CShaderProgram spMain;

CTexture tGold, tSnow;

//--------------------------------------------------------------------
/// \brief Initializes scene before rendering
//--------------------------------------------------------------------

void CSDLOpenGLWindow::InitScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	vboSceneObjects.CreateVBO();
	glGenVertexArrays(1, &uiVAO); // Create one VAO
	glBindVertexArray(uiVAO);

	vboSceneObjects.BindVBO();

	// Add cube to VBO

	for (int i = 0; i < 36; i++)
	{
		vboSceneObjects.AddData(&vCubeVertices[i], sizeof(glm::vec3));
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	// Add pyramid to VBO

	for (int i = 0; i < 12; i++)
	{
		vboSceneObjects.AddData(&vPyramidVertices[i], sizeof(glm::vec3));
		vboSceneObjects.AddData(&vPyramidTexCoords[i % 3], sizeof(glm::vec2));
	}

	// Add ground to VBO

	for (int i = 0; i < 6; i++)
	{
		vboSceneObjects.AddData(&vGround[i], sizeof(glm::vec3));
		vCubeTexCoords[i] *= 5.0f;
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	vboSceneObjects.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions start on zero index, and distance between two consecutive is sizeof whole
	// vertex data (position and tex. coord)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates start right after positon, thus on (sizeof(glm::vec3)) index,
	// and distance between two consecutive is sizeof whole vertex data (position and tex. coord)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	// Load shaders and create shader program

	shVertex.LoadShader("data\\shaders\\tut06\\shader.vert", GL_VERTEX_SHADER);
	shFragment.LoadShader("data\\shaders\\tut06\\shader.frag", GL_FRAGMENT_SHADER);

	spMain.CreateProgram();
	spMain.AddShaderToProgram(&shVertex);
	spMain.AddShaderToProgram(&shFragment);

	spMain.LinkProgram();
	spMain.UseProgram();

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// Finally, load our texture

	tGold.LoadTexture2D("data\\textures\\golddiag.jpg", true);
	tGold.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	tSnow.LoadTexture2D("data\\textures\\snow.jpg", true);
	tSnow.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
}

//--------------------------------------------------------------------
/// \brief  Renders whole scene
//--------------------------------------------------------------------

float fRotationAngleCube = 0.0f, fRotationAnglePyramid = 0.0f;
float fCubeRotationSpeed = 0.0f, fPyramidRotationSpeed = 0.0f;
const float PIover180 = 3.1415f / 180.0f;

void DisplayTextureFiltersInfo()
{
	string sInfoMinification[] =
	{
		"Nearest",
		"Bilinear"
	};
	string sInfoMagnification[] =
	{
		"Nearest",
		"Bilinear",
		"Nearest on closest mipmap",
		"Bilinear on closest mipmap",
		"Trilinear"
	};
	printf("Mag. Filter: %s, Min. Filter: %s\n", sInfoMinification[tSnow.GetMagnificationFilter()].c_str(),
		sInfoMagnification[tSnow.GetMinificationFilter() - 2].c_str());
}

void CSDLOpenGLWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	spMain.UseProgram();
	glm::mat4 mModelView = glm::lookAt(glm::vec3(0, 12, 27), glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 mCurrent;

	spMain.SetUniform("projectionMatrix", GetProjectionMatrix());
	
	glBindVertexArray(uiVAO);

	// Texture binding - we set GL_ACTIVE_TEXTURE0, and then we tell fragment shader,
	// that gSampler variable will fetch data from GL_ACTIVE_TEXTURE0

	spMain.SetUniform("gSampler", 0);
	glEnable(GL_TEXTURE_2D);
	tGold.BindTexture(0);

	// Rendering of cube

	mCurrent = glm::translate(mModelView, glm::vec3(-8.0f, 0.0f, 0.0f));
	mCurrent = glm::scale(mCurrent, glm::vec3(10.0f, 10.0f, 10.0f));
	mCurrent = glm::rotate(mCurrent, fRotationAngleCube, glm::vec3(1.0f, 0.0f, 0.0f));

	spMain.SetUniform("modelViewMatrix", mCurrent);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// Rendering of pyramid

	mCurrent = glm::translate(mModelView, glm::vec3(8.0f, 0.0f, 0.0f));
	mCurrent = glm::scale(mCurrent, glm::vec3(10.0f, 10.0f, 10.0f));
	mCurrent = glm::rotate(mCurrent, fRotationAnglePyramid, glm::vec3(0.0f, 1.0f, 0.0f));
	spMain.SetUniform("modelViewMatrix", mCurrent);

	glDrawArrays(GL_TRIANGLES, 36, 12);

	// Render ground

	tSnow.BindTexture();

	spMain.SetUniform("modelViewMatrix", mModelView);
	glDrawArrays(GL_TRIANGLES, 48, 6);

	// A little interaction for user

	if (Keys::Key(SDL_SCANCODE_UP))
		fCubeRotationSpeed -= sof(60.0f*PIover180);
	if (Keys::Key(SDL_SCANCODE_DOWN))
		fCubeRotationSpeed += sof(60.0f*PIover180);
	if (Keys::Key(SDL_SCANCODE_RIGHT))
		fPyramidRotationSpeed += sof(60.0f*PIover180);
	if (Keys::Key(SDL_SCANCODE_LEFT))
		fPyramidRotationSpeed -= sof(60.0f*PIover180);

	fRotationAngleCube += sof(fCubeRotationSpeed);
	fRotationAnglePyramid += sof(fPyramidRotationSpeed);

	// F1 and F2 change the texture filterings and set window text about that
	if (Keys::Onekey(SDL_SCANCODE_F1))
	{
		tGold.SetFiltering((ETextureFiltering)((int(tGold.GetMagnificationFilter()) + 1) % 2), tGold.GetMinificationFilter());
		tSnow.SetFiltering((ETextureFiltering)((int(tSnow.GetMagnificationFilter()) + 1) % 2), tSnow.GetMinificationFilter());
		DisplayTextureFiltersInfo();
	}
	if (Keys::Onekey(SDL_SCANCODE_F2))
	{
		ETextureFiltering iNewMinFilter = tSnow.GetMinificationFilter() == TEXTURE_FILTER_MIN_TRILINEAR ? TEXTURE_FILTER_MIN_NEAREST : (ETextureFiltering)( (int)tSnow.GetMinificationFilter() + 1);
		tSnow.SetFiltering(tSnow.GetMagnificationFilter(), iNewMinFilter);
		tGold.SetFiltering(tGold.GetMagnificationFilter(), iNewMinFilter);
		DisplayTextureFiltersInfo();
	}
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{

}
