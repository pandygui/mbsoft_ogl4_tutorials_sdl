#include "../common_classes/common_header.h"
#include "../common_classes/SDL_OpenGLApp.h"

#include <windows.h>

#undef main

int main()
{
	bool bFullScreen = MessageBox(NULL, "Would you like to run in fullscreen?", "Fullscreen", MB_ICONQUESTION | MB_YESNO) == IDYES;
	
	if (openGLWindow.CreateOpenGLWindow("01.) Creating OpenGL 4.2 Window with SDL", bFullScreen, 3, 3))
		appMain.AppBody();

	return 0;
}