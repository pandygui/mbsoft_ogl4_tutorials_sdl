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

CVertexBufferObject vboSceneObjects;
GLuint uiVAOs[2]; // First VAO is for textured objects, second for only colored (we must disable texture coordinates attribute)

CShader shVertexTex, shFragmentTex, shVertexCol, shFragmentCol;
CShaderProgram spTextured, spColored;

CTexture tBlueIce, tBox;

CWalkingCamera cCamera;

//--------------------------------------------------------------------
/// \brief Initializes scene before rendering
//--------------------------------------------------------------------

void CSDLOpenGLWindow::InitScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	vboSceneObjects.CreateVBO();
	glGenVertexArrays(2, uiVAOs); // Create one VAO
	glBindVertexArray(uiVAOs[0]);

	vboSceneObjects.BindVBO();

	// Add cube to VBO

	for (int i = 0; i < 36; i++)
	{
		vboSceneObjects.AddData(&vCubeVertices[i], sizeof(glm::vec3));
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	// Add ground to VBO

	for (int i = 0; i < 6; i++)
	{
		vboSceneObjects.AddData(&vGround[i], sizeof(glm::vec3));
		vCubeTexCoords[i] *= 10.0f;
		vboSceneObjects.AddData(&vCubeTexCoords[i % 6], sizeof(glm::vec2));
	}

	vboSceneObjects.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	// Setup another VAO for untextured objects

	glBindVertexArray(uiVAOs[1]);
	// Here enable only first vertex attribute - position (no texture coordinates)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);

	// Load shaders and create shader programs

	shVertexTex.LoadShader("data\\shaders\\tut07\\shaderTex.vert", GL_VERTEX_SHADER);
	shFragmentTex.LoadShader("data\\shaders\\tut07\\shaderTex.frag", GL_FRAGMENT_SHADER);
	shVertexCol.LoadShader("data\\shaders\\tut07\\shaderCol.vert", GL_VERTEX_SHADER);
	shFragmentCol.LoadShader("data\\shaders\\tut07\\shaderCol.frag", GL_FRAGMENT_SHADER);

	spTextured.CreateProgram();
		spTextured.AddShaderToProgram(&shVertexTex);
		spTextured.AddShaderToProgram(&shFragmentTex);
	spTextured.LinkProgram();

	spColored.CreateProgram();
		spColored.AddShaderToProgram(&shVertexCol);
		spColored.AddShaderToProgram(&shFragmentCol);
	spColored.LinkProgram();

	tBlueIce.LoadTexture2D("data\\textures\\blueice.jpg", true);
	tBlueIce.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	tBox.LoadTexture2D("data\\textures\\box.jpg", true);
	tBox.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glClearColor(0.2f, 0.32f, 0.5f, 1.0f);

	cCamera = CWalkingCamera(glm::vec3(0.0f, 0.0f, -20.0f), glm::vec3(0.0f, 0.0f, -19.0f), glm::vec3(0.0f, 1.0f, 0.0f), 15.0f);
}

//--------------------------------------------------------------------
/// \brief  Renders whole scene
//--------------------------------------------------------------------

const float PIover180 = 3.1415f / 180.0f;
float fGlobalAngle;

glm::vec4 vBoxColors[] =
{
	glm::vec4(1.0f, 0.0f, 0.0f, 0.5f), // Red
	glm::vec4(0.0f, 1.0f, 0.0f, 0.1f), // Green
	glm::vec4(0.0f, 0.0f, 1.0f, 0.25f), // Blue
	glm::vec4(0.25f, 0.0f, 0.5f, 0.3f), // Dark Purple
	glm::vec4(1.0f, 0.5f, 0.0f, 0.47f), // Orange
};

void CSDLOpenGLWindow::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 mModelView = cCamera.look();
	glm::mat4 mCurrent = mModelView;

	// First render textured objects

	glEnable(GL_TEXTURE_2D);
	spTextured.UseProgram();
	glBindVertexArray(uiVAOs[0]);

	spTextured.SetUniform("projectionMatrix", GetProjectionMatrix());
	spTextured.SetUniform("modelViewMatrix", mModelView);
	spTextured.SetUniform("vColor", glm::vec4(1, 1, 1, 1));

	// Render ground

	tBlueIce.BindTexture();
	glDrawArrays(GL_TRIANGLES, 36, 6);

	// Render 5 opaque boxes

	tBox.BindTexture();

	for (int i = 0; i < 5; i++)
	{
		float fSign = -1.0f + float(i % 2)*2.0f; // This just returns -1.0f or 1.0f (try to examine this)
		glm::vec3 vPos = glm::vec3(fSign*15.0f, 0.0f, 50.0f - float(i)*25.0f);
		mCurrent = glm::translate(mModelView, vPos);
		mCurrent = glm::scale(mCurrent, glm::vec3(8.0f, 8.0f, 8.0f));
		mCurrent = glm::rotate(mCurrent, fGlobalAngle + float(i)*50.0f* PIover180, glm::vec3(0.0f, 1.0f, 0.0f));
		spTextured.SetUniform("modelViewMatrix", mCurrent);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Now switch to only colored rendering

	glDisable(GL_TEXTURE_2D);
	spColored.UseProgram();
	glBindVertexArray(uiVAOs[1]);

	spColored.SetUniform("projectionMatrix", GetProjectionMatrix());
	
	// Render 5 transparent boxes

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(0); // Disable writing to depth buffer

	for (int i = 0; i < 5; i++)
	{
		float fSign = 1.0f - float(i % 2)*2.0f; // Same case as before -  -1.0f or 1.0f
		glm::vec3 vPos = glm::vec3(fSign*15.0f, 0.0f, 50.0f - float(i)*25.0f);
		mCurrent = glm::translate(mModelView, vPos);
		mCurrent = glm::scale(mCurrent, glm::vec3(8.0f, 8.0f, 8.0f));
		mCurrent = glm::rotate(mCurrent, fGlobalAngle*0.8f + i*30.0f*PIover180, glm::vec3(1.0f, 0.0f, 0.0f)); // Just a variation of first rotating
		spColored.SetUniform("modelViewMatrix", mCurrent);
		spColored.SetUniform("vColor", vBoxColors[i]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glDisable(GL_BLEND);
	glDepthMask(1); // Re-enable writing to depth buffer

	fGlobalAngle += sof(100.0f*PIover180);
	cCamera.Update(this);
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{
	spTextured.DeleteProgram();
	spColored.DeleteProgram();

	shFragmentTex.DeleteShader();
	shFragmentCol.DeleteShader();
	shVertexTex.DeleteShader();
	shVertexCol.DeleteShader();

	glDeleteVertexArrays(2, uiVAOs);
	vboSceneObjects.DeleteVBO();
	tBox.DeleteTexture();
	tBlueIce.DeleteTexture();
}
