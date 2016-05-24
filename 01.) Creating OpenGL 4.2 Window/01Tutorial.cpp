#include "../common_classes/common_header.h"

#include "../common_classes/SDL_OpenGLApp.h"

//--------------------------------------------------------------------
/// \brief Initializes scene before rendering
//--------------------------------------------------------------------

void CSDLOpenGLWindow::InitScene()
{
	// Just set the background color to blue
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
}

//--------------------------------------------------------------------
/// \brief  Renders whole scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::RenderScene()
{
	// Clear color buffer to the background color set in InitScene
	glClear(GL_COLOR_BUFFER_BIT);
}

//--------------------------------------------------------------------
/// \brief Releases OpenGL scene
//--------------------------------------------------------------------

void CSDLOpenGLWindow::ReleaseScene()
{
	// Nothing yet
}