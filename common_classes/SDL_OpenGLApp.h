#pragma once

#include "SDL_OpenGLWindow.h"

//--------------------------------------------------------------------
/// \brief Class for encapsulating application control
//--------------------------------------------------------------------

class CSDLOpenGLApp
{
public:
	CSDLOpenGLApp();

	void HandleSystemEvents();

	void AppBody();
	void RequestExit();

private:
	bool m_bAppActive; // To check if application is active (not minimized)
	bool m_bExitRequested;
};

namespace Keys
{
	bool Key(int iKey);
	bool Onekey(int iKey);
	extern unsigned char* kbstate;
	extern bool kp[256];
}

extern CSDLOpenGLApp appMain;
extern CSDLOpenGLWindow openGLWindow;