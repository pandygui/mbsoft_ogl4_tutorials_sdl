#pragma once

/********************************

Class:		CShader

Purpose:	Wraps OpenGL shader loading
			and compiling.

********************************/

class CShader
{
public:
	CShader();

	bool LoadShader(std::string sFile, int a_iType);
	void DeleteShader();

	bool GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string>* vResult);

	bool IsLoaded();
	GLuint GetShaderID();
private:
	GLuint m_uiShader; // ID of shader
	int m_iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool m_bLoaded; // Whether shader was loaded and compiled
};

/********************************

Class:		CShaderProgram

Purpose:	Wraps OpenGL shader program
			and make its usage easy.

********************************/

class CShaderProgram
{
public:
	CShaderProgram();

	void CreateProgram();
	void DeleteProgram();

	bool AddShaderToProgram(CShader* shShader);
	bool LinkProgram();

	void UseProgram();

	GLuint GetProgramID();

	// Setting vectors
	void SetUniform(std::string sName, glm::vec2* vVectors, int iCount = 1);
	void SetUniform(std::string sName, const glm::vec2 vVector);
	void SetUniform(std::string sName, glm::vec3* vVectors, int iCount = 1);
	void SetUniform(std::string sName, const glm::vec3 vVector);
	void SetUniform(std::string sName, glm::vec4* vVectors, int iCount = 1);
	void SetUniform(std::string sName, const glm::vec4 vVector);

	// Setting floats
	void SetUniform(std::string sName, float* fValues, int iCount = 1);
	void SetUniform(std::string sName, const float fValue);

	// Setting 3x3 matrices
	void SetUniform(std::string sName, glm::mat3* mMatrices, int iCount = 1);
	void SetUniform(std::string sName, const glm::mat3 mMatrix);

	// Setting 4x4 matrices
	void SetUniform(std::string sName, glm::mat4* mMatrices, int iCount = 1);
	void SetUniform(std::string sName, const glm::mat4 mMatrix);

	// Setting integers
	void SetUniform(std::string sName, int* iValues, int iCount = 1);
	void SetUniform(std::string sName, const int iValue);

	// Model and normal matrix setting ispretty common
	void SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4 mModelMatrix);
	void SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4* mModelMatrix);

private:
	GLuint m_uiProgram; // ID of program
	bool m_bLinked; // Whether program was linked and is ready to use
};
