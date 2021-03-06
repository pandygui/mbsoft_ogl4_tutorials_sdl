#include "common_header.h"

#include "shaders.h"

#include <glm/gtc/type_ptr.hpp>

CShader::CShader()
{
	m_bLoaded = false;
}
 
/*-----------------------------------------------

Name:    LoadShader

Params:  sFile - path to a file
         a_iType - type of shader (fragment, vertex, geometry)

Result:	Loads and compiles shader.

/*---------------------------------------------*/

bool CShader::LoadShader(std::string sFile, int a_iType)
{
	std::vector<std::string> sLines;

	if(!GetLinesFromFile(sFile, false, &sLines))
		return false;

	const char** sProgram = new const char*[sLines.size()];
	for (int i = 0; i < int(sLines.size()); i++)
		sProgram[i] = sLines[i].c_str();
	
	m_uiShader = glCreateShader(a_iType);

	glShaderSource(m_uiShader, sLines.size(), sProgram, NULL);
	glCompileShader(m_uiShader);

	delete[] sProgram;

	int iCompilationStatus;
	glGetShaderiv(m_uiShader, GL_COMPILE_STATUS, &iCompilationStatus);

	if(iCompilationStatus == GL_FALSE)
	{
		char sInfoLog[1024];
		int iLogLength;
		glGetShaderInfoLog(m_uiShader, 1024, &iLogLength, sInfoLog);
		SDL_Log("Error! Shader file %s wasn't compiled! The compiler returned:\n\n%s\n", sFile.c_str(), sInfoLog);
		return false;
	}
	m_iType = a_iType;
	m_bLoaded = true;

	return true;
}

/*-----------------------------------------------

Name:    GetLinesFromFile

Params:  sFile - path to a file
         bIncludePart - whether to add include part only
         vResult - vector of strings to store result to

Result:  Loads and adds include part.

/*---------------------------------------------*/

std::vector<std::string> split(std::string s, char t)
{
	std::vector<std::string> res;
	while (true)
	{
		int pos = s.find(t);
		if (pos == -1)
		{
			if(s.size() > 0)
				res.push_back(s);
			break;
		}
		std::string tp = s.substr(0, pos);
		if (tp.size() != 0)
			res.push_back(tp);
		s = s.substr(pos + 1, s.size() - pos - 1);
	}
	return res;
}

std::string UpOneDirectory(const std::string& input, char slashCharacter)
{
	bool bTrailingSlash = input.back() == slashCharacter;
	std::vector<std::string> subPaths = split(input, slashCharacter);
	std::string sResult = "";
	for (int i = 0; i < subPaths.size() - 1; i++)
	{
		if (i)
			sResult += slashCharacter;
		sResult += subPaths[i];
	}
	if (bTrailingSlash && sResult.size() > 0)
		sResult += slashCharacter;

	return sResult;
}

std::string ChangeSlashes(const std::string& s, char slashCharacter)
{
	std::string sResult = s;
	for (char& c : sResult)
	{
		if (c == '\\' || c == '/')
			c = slashCharacter;
	}

	return sResult;
}

bool CShader::GetLinesFromFile(std::string sFile, bool bIncludePart, std::vector<std::string>* vResult)
{
	FILE* fp = fopen(sFile.c_str(), "rt");
	if (!fp)
	{
		printf("File %s not found! (Have you set the working directory of the application to $(SolutionDir)bin/?)\n", sFile.c_str());
		return false;
	}

	std::string sStartDirectory;
	char slashCharacter = '/';
	sFile = ChangeSlashes(sFile, slashCharacter);

	int slashIndex = -1;
	for (int i = sFile.size() - 1; i >= 0; i--)
	{
		if(sFile[i] == '/')
		{
			slashIndex = i;
			slashCharacter = sFile[i];
			break;
		}
	}

	sStartDirectory = sFile.substr(0, slashIndex+1);

	// Get all lines from a file

	char sLine[255];

	bool bInIncludePart = false;

	while(fgets(sLine, 255, fp))
	{
		std::stringstream ss(sLine);
		std::string sFirst;
		ss >> sFirst;
		if(sFirst == "#include")
		{
			std::string sFileName;
			ss >> sFileName;
			if(sFileName.size() > 0 && sFileName[0] == '\"' && sFileName[sFileName.size()-1] == '\"')
			{
				sFileName = ChangeSlashes(sFileName.substr(1, sFileName.size() - 2), slashCharacter);
				std::string sDirectory = sStartDirectory;
				std::vector<std::string> subPaths = split(sFileName, slashCharacter);
				std::string sFinalFileName = "";
				for (const std::string& subPath : subPaths)
				{
					if (subPath == "..")
						sDirectory = UpOneDirectory(sDirectory, slashCharacter);
					else
					{
						if (sFinalFileName.size() > 0)
							sFinalFileName += slashCharacter;
						sFinalFileName += subPath;
					}
				}
				GetLinesFromFile(sDirectory+sFinalFileName, true, vResult);
			}
		}
		else if(sFirst == "#include_part")
			bInIncludePart = true;
		else if(sFirst == "#definition_part")
			bInIncludePart = false;
		else if(!bIncludePart || (bIncludePart && bInIncludePart))
			vResult->push_back(sLine);
	}
	fclose(fp);

	return true;
}

/*-----------------------------------------------

Name:	IsLoaded

Params:	none

Result:	True if shader was loaded and compiled.

/*---------------------------------------------*/

bool CShader::IsLoaded()
{
	return m_bLoaded;
}

/*-----------------------------------------------

Name:	GetShaderID

Params:	none

Result:	Returns ID of a generated shader.

/*---------------------------------------------*/

GLuint CShader::GetShaderID()
{
	return m_uiShader;
}

/*-----------------------------------------------

Name:	DeleteShader

Params:	none

Result:	Deletes shader and frees memory in GPU.

/*---------------------------------------------*/

void CShader::DeleteShader()
{
	if(!IsLoaded())return;
	m_bLoaded = false;
	glDeleteShader(m_uiShader);
}

CShaderProgram::CShaderProgram()
{
	m_bLinked = false;
}

/*-----------------------------------------------

Name:	CreateProgram

Params:	none

Result:	Creates a new program.

/*---------------------------------------------*/

void CShaderProgram::CreateProgram()
{
	m_uiProgram = glCreateProgram();
}

/*-----------------------------------------------

Name:	AddShaderToProgram

Params:	sShader - shader to add

Result:	Adds a shader (like source file) to
		a program, but only compiled one.

/*---------------------------------------------*/

bool CShaderProgram::AddShaderToProgram(CShader* shShader)
{
	if(!shShader->IsLoaded())return false;

	glAttachShader(m_uiProgram, shShader->GetShaderID());

	return true;
}

/*-----------------------------------------------

Name:	LinkProgram

Params:	none

Result:	Performs final linkage of OpenGL program.

/*---------------------------------------------*/

bool CShaderProgram::LinkProgram()
{
	glLinkProgram(m_uiProgram);
	int iLinkStatus;
	glGetProgramiv(m_uiProgram, GL_LINK_STATUS, &iLinkStatus);
	m_bLinked = iLinkStatus == GL_TRUE;
	if(!m_bLinked)
	{
		char sInfoLog[1024];
		int iLogLength;
		glGetProgramInfoLog(m_uiProgram, 1024, &iLogLength, sInfoLog);

		SDL_Log("Error! Shader program wasn't linked! The linker returned:\n\n%s\n", sInfoLog);
		return false;
	}
	return m_bLinked;
}

/*-----------------------------------------------

Name:	DeleteProgram

Params:	none

Result:	Deletes program and frees memory on GPU.

/*---------------------------------------------*/

void CShaderProgram::DeleteProgram()
{
	if(!m_bLinked)
		return;

	m_bLinked = false;
	glDeleteProgram(m_uiProgram);
}

/*-----------------------------------------------

Name:	UseProgram

Params:	none

Result:	Tells OpenGL to use this program.

/*---------------------------------------------*/

void CShaderProgram::UseProgram()
{
	if(m_bLinked)
		glUseProgram(m_uiProgram);
}

/*-----------------------------------------------

Name:	GetProgramID

Params:	none

Result:	Returns OpenGL generated shader program ID.

/*---------------------------------------------*/

GLuint CShaderProgram::GetProgramID()
{
	return m_uiProgram;
}

/*-----------------------------------------------

Name:	UniformSetters

Params:	yes, there are :)

Result:	These set of functions set most common
		types of uniform variables.

/*---------------------------------------------*/

// Setting floats

void CShaderProgram::SetUniform(std::string sName, float* fValues, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}

void CShaderProgram::SetUniform(std::string sName, const float fValue)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}

// Setting vectors

void CShaderProgram::SetUniform(std::string sName, glm::vec2* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(std::string sName, const glm::vec2 vVector)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(std::string sName, glm::vec3* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(std::string sName, const glm::vec3 vVector)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void CShaderProgram::SetUniform(std::string sName, glm::vec4* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void CShaderProgram::SetUniform(std::string sName, const glm::vec4 vVector)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

// Setting 3x3 matrices

void CShaderProgram::SetUniform(std::string sName, glm::mat3* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, iCount, false, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(std::string sName, const glm::mat3 mMatrix)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix3fv(iLoc, 1, false, (GLfloat*)&mMatrix);
}

// Setting 4x4 matrices

void CShaderProgram::SetUniform(std::string sName, glm::mat4* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, false, (GLfloat*)mMatrices);
}

void CShaderProgram::SetUniform(std::string sName, const glm::mat4 mMatrix)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, false, (GLfloat*)&mMatrix);
}

// Setting integers

void CShaderProgram::SetUniform(std::string sName, int* iValues, int iCount)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}

void CShaderProgram::SetUniform(std::string sName, const int iValue)
{
	int iLoc = glGetUniformLocation(m_uiProgram, sName.c_str());
	glUniform1i(iLoc, iValue);
}

void CShaderProgram::SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4 mModelMatrix)
{
	SetUniform(sModelMatrixName, mModelMatrix);
	SetUniform(sNormalMatrixName, glm::transpose(glm::inverse(glm::mat3(mModelMatrix))));
}

void CShaderProgram::SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4* mModelMatrix)
{
	SetModelAndNormalMatrix(sModelMatrixName, sNormalMatrixName, *mModelMatrix);
}