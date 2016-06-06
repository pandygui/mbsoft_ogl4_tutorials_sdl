#pragma once

/********************************

Class:		CShader

Purpose:	Wraps OpenGL shader loading
			and compiling.

********************************/

class CShader
{
public:
	bool LoadShader(string sFile, int a_iType);
	void DeleteShader();

	bool GetLinesFromFile(string sFile, bool bIncludePart, vector<string>* vResult);

	bool IsLoaded();
	GLuint GetShaderID();

	CShader();

private:
	GLuint uiShader; // ID of shader
	int iType; // GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
	bool bLoaded; // Whether shader was loaded and compiled
};

/********************************

Class:		CShaderProgram

Purpose:	Wraps OpenGL shader program
			and make its usage easy.

********************************/

class CShaderProgram
{
public:
	void CreateProgram();
	void DeleteProgram();

	bool AddShaderToProgram(CShader* shShader);
	bool LinkProgram();

	void UseProgram();

	GLuint GetProgramID();

	// Setting vectors
	void SetUniform(string sName, glm::vec2* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec2 vVector);
	void SetUniform(string sName, glm::vec3* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec3 vVector);
	void SetUniform(string sName, glm::vec4* vVectors, int iCount = 1);
	void SetUniform(string sName, const glm::vec4 vVector);

	// Setting floats
	void SetUniform(string sName, float* fValues, int iCount = 1);
	void SetUniform(string sName, const float fValue);

	// Setting 3x3 matrices
	void SetUniform(string sName, glm::mat3* mMatrices, int iCount = 1);
	void SetUniform(string sName, const glm::mat3 mMatrix);

	// Setting 4x4 matrices
	void SetUniform(string sName, glm::mat4* mMatrices, int iCount = 1);
	void SetUniform(string sName, const glm::mat4 mMatrix);

	// Setting integers
	void SetUniform(string sName, int* iValues, int iCount = 1);
	void SetUniform(string sName, const int iValue);

	// Model and normal matrix setting ispretty common
	void SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix);
	void SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4* mModelMatrix);

	CShaderProgram();

private:
	GLuint uiProgram; // ID of program
	bool bLinked; // Whether program was linked and is ready to use
};