#pragma once

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

/********************************

Class:		CTexture

Purpose:	Wraps OpenGL texture
			object and performs
			their loading.

********************************/

class CTexture
{
public:
	CTexture();

	void CreateEmptyTexture(int iWidth, int iHeight, GLenum format);
	void CreateFromData(GLbyte* bData, int iWidth, int iHeight, int iBPP, GLenum format, bool bGenerateMipMaps = false);
	
	bool LoadTexture2D(string sPath, bool bGenerateMipMaps = false);
	void BindTexture(int iTextureUnit = 0);

	void SetFiltering(ETextureFiltering a_tfMagnification, ETextureFiltering a_tfMinification);

	void SetSamplerParameter(GLenum parameter, GLenum value);

	ETextureFiltering GetMinificationFilter();
	ETextureFiltering GetMagnificationFilter();

	int GetWidth();
	int GetHeight();
	int GetBPP();

	GLuint GetTextureID();
	string GetPath();

	void DeleteTexture();
private:

	int m_iWidth, m_iHeight, m_iBPP; // Texture width, height, and bytes per pixel
	GLuint m_uiTexture; // Texture name
	GLuint m_uiSampler; // Sampler name

	ETextureFiltering m_tfMinification, m_tfMagnification;

	string m_sPath;
};