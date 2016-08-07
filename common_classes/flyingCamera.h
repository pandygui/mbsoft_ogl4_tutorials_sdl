#pragma once

#include "../common_classes/SDL_OpenGLWindow.h"

class CFlyingCamera
{
public:
	// Constructors
	CFlyingCamera();
	CFlyingCamera(glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity);

	// Main functions
	void RotateWithMouse(CSDLOpenGLWindow* wnd);
	void Update(CSDLOpenGLWindow* wnd);
	glm::mat4 Look();

	void SetMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight);
	void ResetMouse(CSDLOpenGLWindow* wnd);

	// Functions that get viewing angles
	float GetAngleX(), GetAngleY();

	glm::vec3 GetEye();

private:
	glm::vec3 m_vEye, m_vView, vUp;
	float m_fSpeed;
	float m_fSensitivity; // How many degrees to rotate per pixel moved by mouse (nice value is 0.10)

	int m_iMouseX, iMouseY; // For mouse rotation
	int m_iForw, m_iBack, m_iLeft, m_iRight;
};