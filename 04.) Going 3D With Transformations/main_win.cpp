#include "../common_classes/common_header.h"
#include "../common_classes/SDL_OpenGLApp.h"

#include <windows.h>

#undef main

int main()
{
	bool bFullScreen = MessageBox(NULL, "Would you like to run in fullscreen?", "Fullscreen", MB_ICONQUESTION | MB_YESNO) == IDYES;
	
	if (openGLWindow.CreateOpenGLWindow("04.) Going 3D With Transformations - Tutorial by Michal Bubnar (www.mbsoftworks.sk)", bFullScreen, 3, 3))
		appMain.AppBody();

	return 0;
}