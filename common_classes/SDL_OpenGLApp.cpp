#include "common_header.h"
#include "SDL_OpenGLApp.h"

CSDLOpenGLWindow openGLWindow;
CSDLOpenGLApp appMain;

bool Keys::kp[256] = {false};
unsigned char* Keys::kbstate = NULL;

//--------------------------------------------------------------------
/// \brief Returns true if Key is pressed
/// \param iKey SDL virtual key code
//--------------------------------------------------------------------

bool Keys::Key(int iKey)
{
	if (kbstate == NULL)
		return false;
	return kbstate[iKey] ? true : false;
}

//--------------------------------------------------------------------
/// \brief Return true if Key was pressed, but only once
/// (the Key must be released)
/// \param iKey SDL virtual key code
//--------------------------------------------------------------------

bool Keys::Onekey(int iKey)
{
	if (Key(iKey) && !kp[iKey]) { kp[iKey] = true; return true; }
	if (!Key(iKey))kp[iKey] = false;
	return false;
}

CSDLOpenGLApp::CSDLOpenGLApp()
{
	m_bExitRequested = false;
	m_bAppActive = true;
}

//--------------------------------------------------------------------
/// \brief Handles several system events
/// (the Key must be released)
/// \param iKey SDL virtual key code
//--------------------------------------------------------------------

void CSDLOpenGLApp::HandleSystemEvents()
{
	Keys::kbstate = (unsigned char*)SDL_GetKeyboardState(NULL);
	SDL_Event e;
	while(SDL_PollEvent( &e ) != 0)
	{
		//User requests quit
		if( e.type == SDL_QUIT )
			m_bExitRequested = true;
		else if(e.type == SDL_WINDOWEVENT)
		{
			switch(e.window.event)
			{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					int w = e.window.data1;
					int h = e.window.data2;
					openGLWindow.ResizeOpenGLViewportFull();
					openGLWindow.SetProjection3D(45.0f, float(w)/float(h), 0.5f, 1000.0f);
					openGLWindow.SetOrtho2D(w, h);
					break;
				}
			}
		}
		else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
			m_bExitRequested = true;
	}
}

//--------------------------------------------------------------------
/// \brief Just an application infinite while loop until user quits
//--------------------------------------------------------------------

void CSDLOpenGLApp::AppBody()
{
	openGLWindow.ResetFPS();

	while(!m_bExitRequested)
	{
		HandleSystemEvents();
		if(m_bAppActive)
			openGLWindow.Render();
	}

	openGLWindow.ReleaseOpenGLControl();
	SDL_Quit();
}

//--------------------------------------------------------------------
/// \brief Requests application exit
//--------------------------------------------------------------------

void CSDLOpenGLApp::RequestExit()
{
	m_bExitRequested = true;
}
