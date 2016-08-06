#include "common_header.h"

#include "texture.h"

#include <FreeImage.h>

#pragma comment(lib, "FreeImage.lib")

CTexture::CTexture()
{
	m_uiTexture = 0;
	m_uiSampler = 0;
}

/*-----------------------------------------------

Name:	CreateEmptyTexture

Params:	a_iWidth, a_iHeight - dimensions
		format - format of data

Result:	Creates texture from provided data.

/*---------------------------------------------*/

void CTexture::CreateEmptyTexture(int iWidth, int iHeight, GLenum format)
{
	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);
	if(format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	// We must handle this because of internal format parameter
	else if(format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	
	glGenSamplers(1, &m_uiSampler);
}

/*-----------------------------------------------

Name:	CreateFromData

Params:	a_sPath - path to the texture
		format - format of data
		bGenerateMipMaps - whether to create mipmaps

Result:	Creates texture from provided data.

/*---------------------------------------------*/

void CTexture::CreateFromData(void* bData, int iWidth, int iHeight, int iBPP, GLenum format, bool bGenerateMipMaps)
{
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &m_uiTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);
	if(format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	// We must handle this because of internal format parameter
	else if(format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, iWidth, iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	if(bGenerateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &m_uiSampler);

	m_sPath = "";
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iBPP = iBPP;
}

/*-----------------------------------------------

Name:	LoadTexture2D

Params:	a_sPath - path to the texture
		bGenerateMipMaps - whether to create mipmaps

Result:	Loads texture from a file, supports most
		graphics formats.

/*---------------------------------------------*/

bool CTexture::LoadTexture2D(string sPath, bool bGenerateMipMaps)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(sPath.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(sPath.c_str());
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, sPath.c_str());
	if(!dib)
		return false;

	GLbyte* bDataPointer = (GLbyte*)FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if(bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format = 0;
	int bada = FreeImage_GetBPP(dib);
	if(FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	if(FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	if(FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
	if(format != 0)
	CreateFromData(bDataPointer, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, bGenerateMipMaps);
	
	FreeImage_Unload(dib);

	if(format == 0)
		return false;

	m_sPath = sPath;

	return true; // Success
}

void CTexture::SetSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(m_uiSampler, parameter, value);
}

/*-----------------------------------------------

Name:	SetFiltering

Params:	tfMagnification - mag. filter, must be from
							ETextureFiltering enum
		tfMinification - min. filter, must be from
							ETextureFiltering enum

Result:	Sets magnification and minification
			texture filter.

/*---------------------------------------------*/

void CTexture::SetFiltering(ETextureFiltering a_tfMagnification, ETextureFiltering a_tfMinification)
{
	glBindSampler(0, m_uiSampler);

	// Set magnification filter
	if(a_tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if(a_tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if(a_tfMinification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if(a_tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(m_uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	m_tfMinification = a_tfMinification;
	m_tfMagnification = a_tfMagnification;
}

/*-----------------------------------------------

Name:	BindTexture

Params:	iTextureUnit - texture unit to bind texture to

Result:	Guess what it does :)

/*---------------------------------------------*/

void CTexture::BindTexture(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_uiTexture);
	glBindSampler(iTextureUnit, m_uiSampler);
}

/*-----------------------------------------------

Name:	DeleteTexture

Params:	none

Result:	Frees all memory used by texture.

/*---------------------------------------------*/

void CTexture::DeleteTexture()
{
	if (m_uiTexture == 0)
		return;

	glDeleteSamplers(1, &m_uiSampler);
	glDeleteTextures(1, &m_uiTexture);
	m_uiSampler = 0;
	m_uiTexture = 0;
}

/*-----------------------------------------------

Name:	Getters

Params:	none

Result:	... They get something :D

/*---------------------------------------------*/

ETextureFiltering CTexture::GetMinificationFilter()
{
	return m_tfMinification;
}

ETextureFiltering CTexture::GetMagnificationFilter()
{
	return m_tfMagnification;
}

int CTexture::GetWidth()
{
	return m_iWidth;
}

int CTexture::GetHeight()
{
	return m_iHeight;
}

int CTexture::GetBPP()
{
	return m_iBPP;
}

GLuint CTexture::GetTextureID()
{
	return m_uiTexture;
}

string CTexture::GetPath()
{
	return m_sPath;
}
