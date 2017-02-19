#pragma once

//--------------------------------------------------------------------
/// \brief Provides convenient usage of OpenGL
//--------------------------------------------------------------------

class CSDLOpenGLWindow
{
public:
	CSDLOpenGLWindow();

	bool CreateOpenGLWindow(std::string sWindowTitle, bool bFullScreen, int iMajorVersion, int iMinorVersion);
	
	void ResizeOpenGLViewportFull();
	void SetProjection3D(float fFOV, float fAspectRatio, float fNear, float fFar);
	void SetOrtho2D(int width, int height);

	glm::mat4& GetProjectionMatrix();
	glm::mat4& GetOrthoMatrix();

	void Render();
	void ReleaseOpenGLControl();

	void MakeCurrent();
	void SwapBuffers();

	void SetVerticalSynchronization(bool bEnabled);

	int GetFPS();
	int GetViewportWidth();
	int GetViewportHeight();

	float sof(float fVal);
	void ResetFPS();
	bool IsFPSUpdated();

	// Actual code for rendering is inside these functions
	void InitScene();
	void RenderScene();
	void ReleaseScene();

	SDL_Window* GetSDLWindow();

private:
	SDL_Window* m_window;
	SDL_GLContext m_glContext;
	int m_iMajorVersion, m_iMinorVersion;

	// Used for FPS calculation
	int m_iFPSCount, m_iCurrentFPS;
	clock_t m_lastSecond;
	clock_t tLastFrame;
	float fFrameInterval;

	glm::mat4 m_matProjection; // Matrix for perspective projection
	glm::mat4 m_matOrtho; // Matrix for orthographic 2D projection

	int m_iViewportWidth, m_iViewportHeight; // Viewport parameters

	void _UpdateFPS();
	bool m_bFPSUpdated; // Flag that tells, that FPS counter has been updated (one second has passed)
};