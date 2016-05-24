#pragma comment(lib, "opengl32.lib")

#pragma comment(lib, "glew32.lib")

#include "common_header.h"

#include "SDL_OpenGLWindow.h"

#include <gl/wglew.h>
#include <glm/gtc/matrix_transform.hpp>

#pragma comment(lib, "SDL2.lib")

CSDLOpenGLWindow::CSDLOpenGLWindow()
{
	m_iFPSCount = 0;
	m_iCurrentFPS = 0;

	m_window = NULL;
	m_glContext = NULL;
}

//--------------------------------------------------------------------
/// \brief Initializes OpenGL rendering context of specified version
/// \param sWindowTitle Main window title
/// \param bFullScreen True if window should be displayed as fullscreen
/// \param iMajorVersion Major version of OpenGL
/// \param iMinorVersion Minor version of OpenGL
//--------------------------------------------------------------------

bool CSDLOpenGLWindow::CreateOpenGLWindow(string sWindowTitle, bool bFullScreen, int iMajorVersion, int iMinorVersion)
{
	if(!bFullScreen)
		m_window = SDL_CreateWindow(sWindowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	else
	{
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);
		m_window = SDL_CreateWindow(sWindowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mode.w, mode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	SDL_ClearError();
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, iMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, iMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	m_glContext = SDL_GL_CreateContext(m_window);

	SDL_GL_MakeCurrent(m_window, m_glContext);

	const char* error = SDL_GetError();
	if(*error != '\0')
	{
		SDL_Log("OpenGL %d.%d is not supported! Please download latest GPU drivers!", iMajorVersion, iMinorVersion);
		return false;
	}

	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Couldn't initialize GLEW!", iMajorVersion, iMinorVersion);
		return false;
	}

	InitScene();

	SDL_MaximizeWindow(m_window);
	m_iMajorVersion = iMajorVersion;
	m_iMinorVersion = iMinorVersion;

	return true;
}

//--------------------------------------------------------------------
/// \brief Resizes OpenGL viewport to full window
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ResizeOpenGLViewportFull()
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);
	glViewport(0, 0, w, h);
	m_iViewportWidth = w;
	m_iViewportHeight = h;
}

//--------------------------------------------------------------------
/// \brief Calculates projection matrix and stores it
/// \param fFOV field of view angle
/// \param fAspectRatio aspect ratio of width/height
/// \param fNear distance of near clipping plane
/// \param fFar distance of far clipping plane
//--------------------------------------------------------------------

void CSDLOpenGLWindow::SetProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar)
{
	m_matProjection = glm::perspective(fFOV, fAspectRatio, fNear, fFar);
}

//--------------------------------------------------------------------
/// \brief Calculates ortho 2D projection matrix and stores it
/// \param width width of window
/// \param height height of window
//--------------------------------------------------------------------

void CSDLOpenGLWindow::SetOrtho2D(int width, int height)
{
	m_matOrtho = glm::ortho(0.0f, float(width), 0.0f, float(height));
}

//--------------------------------------------------------------------
/// \brief Retrieves projection matrix
//--------------------------------------------------------------------

glm::mat4& CSDLOpenGLWindow::GetProjectionMatrix()
{
	return m_matProjection;
}

//--------------------------------------------------------------------
/// \brief Retrieves ortho matrix
//--------------------------------------------------------------------

glm::mat4& CSDLOpenGLWindow::GetOrthoMatrix()
{
	return m_matOrtho;
}

//--------------------------------------------------------------------
/// \brief Makes current device context and OpenGL rendering context
/// to those associated with OpenGL control
//--------------------------------------------------------------------

void CSDLOpenGLWindow::MakeCurrent()
{
	SDL_GL_MakeCurrent(m_window, m_glContext);
}

//--------------------------------------------------------------------
/// \brief Swaps front and back rendering buffer
//--------------------------------------------------------------------

void CSDLOpenGLWindow::SwapBuffers()
{
	SDL_GL_SwapWindow(m_window);
}

//--------------------------------------------------------------------
/// \brief Performs rendering and calculates FPS
//--------------------------------------------------------------------

void CSDLOpenGLWindow::Render()
{
	RenderScene();
	_UpdateFPS();
	SwapBuffers();
}

//--------------------------------------------------------------------
/// \brief Calls release function and deletes rendering context
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseOpenGLControl()
{
	if(m_glContext)
	ReleaseScene();

	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
}

//--------------------------------------------------------------------
/// \brief Enables or disables vertical synchronization
/// \param bEnabled True if V-Sync should be enabled or false otherwise
//--------------------------------------------------------------------

void CSDLOpenGLWindow::SetVerticalSynchronization(bool bEnabled)
{
	SDL_GL_SetSwapInterval(bEnabled ? 1 : 0);
}

int CSDLOpenGLWindow::GetFPS()
{
	return m_iFPSCount;
}

int CSDLOpenGLWindow::GetViewportWidth()
{
	return m_iViewportWidth;
}

int CSDLOpenGLWindow::GetViewportHeight()
{
	return m_iViewportHeight;
}

float CSDLOpenGLWindow::sof(float fVal)
{
	return fVal*fFrameInterval;
}

void CSDLOpenGLWindow::ResetFPS()
{
	tLastFrame = clock();
	fFrameInterval = 0.0f;
}

//--------------------------------------------------------------------
/// \brief Updates and recalculates FPS counter after frame
//--------------------------------------------------------------------

void CSDLOpenGLWindow::_UpdateFPS()
{
	m_iCurrentFPS++;
	clock_t tCur = clock();
	fFrameInterval = float(tCur - tLastFrame) / float(CLOCKS_PER_SEC);
	tLastFrame = tCur;

	if ((tCur - m_lastSecond) >= CLOCKS_PER_SEC)
	{
		m_lastSecond += CLOCKS_PER_SEC;
		m_iFPSCount = m_iCurrentFPS;
		m_iCurrentFPS = 0;
	}
}