#include "../common_classes/common_header.h"

#include "../common_classes/flyingCamera.h"
#include "../common_classes/SDL_OpenGLApp.h"

#include <glm/gtx/rotate_vector.hpp>

const float PI = float(atan(1.0)*4.0);

CFlyingCamera::CFlyingCamera()
{
	m_vEye = glm::vec3(0.0f, 0.0f, 0.0f);
	m_vView = glm::vec3(0.0f, 0.0, -1.0f);
	vUp = glm::vec3(0.0f, 1.0f, 0.0f);
	m_fSpeed = 25.0f;
	m_fSensitivity = 0.1f;
}

CFlyingCamera::CFlyingCamera(glm::vec3 a_vEye, glm::vec3 a_vView, glm::vec3 a_vUp, float a_fSpeed, float a_fSensitivity)
{
	m_vEye = a_vEye; m_vView = a_vView; vUp = a_vUp;
	m_fSpeed = a_fSpeed;
	m_fSensitivity = a_fSensitivity;
}

/*-----------------------------------------------

Name:	rotateWithMouse

Params:	none

Result:	Checks for moving of mouse and rotates
		camera.

/*---------------------------------------------*/

void CFlyingCamera::RotateWithMouse(CSDLOpenGLWindow* wnd)
{
	SDL_GetGlobalMouseState(&m_iMouseX, &iMouseY);
	int iPosX, iPosY, iWidth, iHeight;
	SDL_GetWindowPosition(wnd->GetSDLWindow(), &iPosX, &iPosY);
	SDL_GetWindowSize(wnd->GetSDLWindow(), &iWidth, &iHeight);

	int iCentX = (iPosX+iWidth)>>1,
		iCentY = (iPosY+iHeight)>>1;

	float deltaX = (float)(iCentX-m_iMouseX)*m_fSensitivity;
	float deltaY = (float)(iCentY-iMouseY)*m_fSensitivity;

	static const float PIover180 = 3.1415f / 180.0f;

	if(deltaX != 0.0f)
	{
		m_vView -= m_vEye;
		m_vView = glm::rotate(m_vView, deltaX*PIover180, glm::vec3(0.0f, 1.0f, 0.0f));
		m_vView += m_vEye;
	}
	if(deltaY != 0.0f)
	{
		glm::vec3 vAxis = glm::cross(m_vView-m_vEye, vUp);
		vAxis = glm::normalize(vAxis);
		float fAngle = deltaY;
		float fNewAngle = fAngle+GetAngleX();
		if(fNewAngle > -89.80f && fNewAngle < 89.80f)
		{
			m_vView -= m_vEye;
			m_vView = glm::rotate(m_vView, deltaY*PIover180, vAxis);
			m_vView += m_vEye;
		}
	}
	SDL_WarpMouseGlobal(iCentX, iCentY);
}

/*-----------------------------------------------

Name:	GetAngleY

Params:	none

Result:	Gets Y angle of camera (head turning left
		and right).

/*---------------------------------------------*/

float CFlyingCamera::GetAngleY()
{
	glm::vec3 vDir = m_vView-m_vEye; vDir.y = 0.0f;
	glm::normalize(vDir);
	float fAngle = acos(glm::dot(glm::vec3(0, 0, -1), vDir))*(180.0f/PI);
	if(vDir.x < 0)fAngle = 360.0f-fAngle;
	return fAngle;
}

glm::vec3 CFlyingCamera::GetEye()
{
	return m_vEye;
}

/*-----------------------------------------------

Name:	GetAngleX

Params:	none

Result:	Gets X angle of camera (head turning up
		and down).

/*---------------------------------------------*/

float CFlyingCamera::GetAngleX()
{
	glm::vec3 vDir = m_vView-m_vEye;
	vDir = glm::normalize(vDir);
	glm::vec3 vDir2 = vDir; vDir2.y = 0.0f;
	vDir2 = glm::normalize(vDir2);
	float fAngle =  acos(glm::dot(vDir2, vDir))*(180.0f/PI);
	if(vDir.y < 0)fAngle *= -1.0f;
	return fAngle;
}

/*-----------------------------------------------

Name:	SetMovingKeys

Params:	a_iForw - move forward Key
		a_iBack - move backward Key
		a_iLeft - strafe left Key
		a_iRight - strafe right Key

Result:	Sets Keys for moving camera.

/*---------------------------------------------*/

void CFlyingCamera::SetMovingKeys(int a_iForw, int a_iBack, int a_iLeft, int a_iRight)
{
	m_iForw = a_iForw;
	m_iBack = a_iBack;
	m_iLeft = a_iLeft;
	m_iRight = a_iRight;
}

/*-----------------------------------------------

Name:	Update

Params:	none

Result:	Performs updates of camera - moving and
		rotating.

/*---------------------------------------------*/

void CFlyingCamera::Update(CSDLOpenGLWindow* wnd)
{
	RotateWithMouse(wnd);

	// Get view direction
	glm::vec3 vMove = m_vView-m_vEye;
	vMove = glm::normalize(vMove);
	vMove *= m_fSpeed;

	glm::vec3 vStrafe = glm::cross(m_vView-m_vEye, vUp);
	vStrafe = glm::normalize(vStrafe);
	vStrafe *= m_fSpeed;

	int iMove = 0;
	glm::vec3 vMoveBy;
	// Get vector of move
	if(Keys::Key(m_iForw))vMoveBy += vMove*wnd->sof(1.0f);
	if(Keys::Key(m_iBack))vMoveBy -= vMove*wnd->sof(1.0f);
	if(Keys::Key(m_iLeft))vMoveBy -= vStrafe*wnd->sof(1.0f);
	if(Keys::Key(m_iRight))vMoveBy += vStrafe*wnd->sof(1.0f);
	m_vEye += vMoveBy; m_vView += vMoveBy;
}

/*-----------------------------------------------

Name:	ResetMouse

Params:	none

Result:	Sets mouse cursor back to the center of
		window.

/*---------------------------------------------*/

void CFlyingCamera::ResetMouse(CSDLOpenGLWindow* wnd)
{
	int iPosX, iPosY, iWidth, iHeight;
	SDL_GetWindowPosition(wnd->GetSDLWindow(), &iPosX, &iPosY);
	SDL_GetWindowSize(wnd->GetSDLWindow(), &iWidth, &iHeight);
	int iCentX = (iPosX + iWidth) >> 1,
		iCentY = (iPosY + iHeight) >> 1;
	SDL_WarpMouseGlobal(iCentX, iCentY);
}

/*-----------------------------------------------

Name:	Look

Params:	none

Result:	Returns proper modelview matrix to make
		camera look.

/*---------------------------------------------*/

glm::mat4 CFlyingCamera::Look()
{
	return glm::lookAt(m_vEye, m_vView, vUp);
}