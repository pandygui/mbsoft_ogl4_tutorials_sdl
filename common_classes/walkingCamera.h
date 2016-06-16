#pragma once

#include "../common_classes/SDL_OpenGLWindow.h"

/********************************

Class:	CWalkingCamera

Purpose:	Camera that can walk
			around the scene.

********************************/

class CWalkingCamera
{
public:
	glm::mat4 look();
	void Update(CSDLOpenGLWindow* wnd);

	void rotateViewY(float fAngle);
	void move(float fBy);

	CWalkingCamera();
	CWalkingCamera(glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed);

private:
	glm::vec3 vEye, vView, vUp;
	float fSpeed;
};