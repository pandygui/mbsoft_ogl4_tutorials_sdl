#include "../common_classes/common_header.h"

#include "../common_classes/freeTypeFont_v1.h"

#include <glm/gtc/matrix_transform.hpp>

#pragma comment(lib, "freetype.lib")

using namespace std;

CFreeTypeFont::CFreeTypeFont()
{
	m_bLoaded = false;
}

/*-----------------------------------------------

Name:	createChar

Params:	iIndex - character index in Unicode.

Result:	Creates one single character (its
		texture).

/*---------------------------------------------*/

inline int next_p2(int n){int res = 1; while(res < n)res <<= 1; return res;}

void CFreeTypeFont::CreateChar(int iIndex)
{
	FT_Load_Glyph(m_ftFace, FT_Get_Char_Index(m_ftFace, iIndex), FT_LOAD_DEFAULT);

	FT_Render_Glyph(m_ftFace->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &m_ftFace->glyph->bitmap;

	int iW = pBitmap->width, iH = pBitmap->rows;
	int iTW = next_p2(iW), iTH = next_p2(iH);

	GLubyte* bData = new GLubyte[iTW*iTH];
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch < iTH; ch++)for(int cw = 0; cw < iTW; cw++)
		bData[ch*iTW+cw] = (ch >= iH || cw >= iW) ? 0 : pBitmap->buffer[(iH-ch-1)*iW+cw];
 
	// And create a texture from it

	m_tCharTextures[iIndex].CreateFromData(bData, iTW, iTH, 8, GL_DEPTH_COMPONENT, false);
	m_tCharTextures[iIndex].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

	m_tCharTextures[iIndex].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_tCharTextures[iIndex].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	m_iAdvX[iIndex] = m_ftFace->glyph->advance.x>>6;
	m_iBearingX[iIndex] = m_ftFace->glyph->metrics.horiBearingX>>6;
	m_iCharWidth[iIndex] = m_ftFace->glyph->metrics.width>>6;

	m_iAdvY[iIndex] = (m_ftFace->glyph->metrics.height - m_ftFace->glyph->metrics.horiBearingY)>>6;
	m_iBearingY[iIndex] = m_ftFace->glyph->metrics.horiBearingY>>6;
	m_iCharHeight[iIndex] = m_ftFace->glyph->metrics.height>>6;

	iNewLine = max(iNewLine, int(m_ftFace->glyph->metrics.height>>6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec2 vQuad[] =
	{
		glm::vec2(0.0f, float(-m_iAdvY[iIndex]+iTH)),
		glm::vec2(0.0f, float(-m_iAdvY[iIndex])),
		glm::vec2(float(iTW), float(-m_iAdvY[iIndex]+iTH)),
		glm::vec2(float(iTW), float(-m_iAdvY[iIndex]))
	};
	glm::vec2 vTexQuad[] = {glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)};

	// Add this char to VBO
	for (int i = 0; i < 4; i++)
	{
		m_vboData.AddData(&vQuad[i], sizeof(glm::vec2));
		m_vboData.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}
	delete[] bData;
}

/*-----------------------------------------------

Name:	loadFont

Params:	sFile - path to font file
		iPXSize - desired font pixel size

Result:	Loads whole font.

/*---------------------------------------------*/

bool CFreeTypeFont::LoadFont(string sFile, int iPXSize)
{
	bool bError = FT_Init_FreeType(&m_ftLib);
	
	bError = FT_New_Face(m_ftLib, sFile.c_str(), 0, &m_ftFace);
	if(bError)return false;
	FT_Set_Pixel_Sizes(m_ftFace, iPXSize, iPXSize);
	m_iLoadedPixelSize = iPXSize;

	glGenVertexArrays(1, &m_uiVAO);
	glBindVertexArray(m_uiVAO);
	m_vboData.CreateVBO();
	m_vboData.BindVBO();

	for (int i = 0; i < 128; i++)CreateChar(i);
	m_bLoaded = true;

	FT_Done_Face(m_ftFace);
	FT_Done_FreeType(m_ftLib);
	
	m_vboData.UploadDataToGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2)*2, (void*)(sizeof(glm::vec2)));
	return true;
}

/*-----------------------------------------------

Name:	loadSystemFont

Params:	sName - system font name
iPXSize - desired font pixel size

Result:	Loads system font (from system Fonts
		directory).

/*---------------------------------------------*/

bool CFreeTypeFont::LoadSystemFont(string sName, int iPXSize)
{
	char buf[512];
	//TODO: SDL function to get windows directory
	//GetWindowsDirectory(buf, 512);
	strcpy(buf, "C:\\Windows");
	std::string sPath = buf;
	sPath += "\\Fonts\\";
	sPath += sName;

	return LoadFont(sPath, iPXSize);
}

/*-----------------------------------------------

Name:	GetTextWidth

Params:	sText - text to get width of
iPXSize - it's printed size

Result:	Returns width as number of pixels the
text will occupy.

/*---------------------------------------------*/

int CFreeTypeFont::GetTextWidth(string sText, int iPXSize)
{
	int iResult = 0;
	for (int i = 0; i < int(sText.size()); i++)
		iResult += m_iAdvX[sText[i]];

	return iResult*iPXSize / m_iLoadedPixelSize;
}

/*-----------------------------------------------

Name:	print

Params:	sText - text to print
		x, y - 2D position
		iPXSize - printed text size

Result:	Prints text at specified position
		with specified pixel size.

/*---------------------------------------------*/

void CFreeTypeFont::Print(string sText, int x, int y, int iPXSize)
{
	if(!m_bLoaded)return;

	glBindVertexArray(m_uiVAO);
	m_shShaderProgram->SetUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if(iPXSize == -1)iPXSize = m_iLoadedPixelSize;
	float fScale = float(iPXSize)/float(m_iLoadedPixelSize);
	for (int i = 0; i < int(sText.size()); i++)
	{
		if(sText[i] == '\n')
		{
			iCurX = x;
			iCurY -= iNewLine*iPXSize/m_iLoadedPixelSize;
			continue;
		}
		int iIndex = int(sText[i]);
		iCurX += m_iBearingX[iIndex]*iPXSize/m_iLoadedPixelSize;
		if(sText[i] != ' ')
		{
			m_tCharTextures[iIndex].BindTexture();
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX), float(iCurY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(fScale));
			m_shShaderProgram->SetUniform("modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, iIndex*4, 4);
		}

		iCurX += (m_iAdvX[iIndex]-m_iBearingX[iIndex])*iPXSize/m_iLoadedPixelSize;
	}
	glDisable(GL_BLEND);
}

/*-----------------------------------------------

Name:	PrintFormatted

Params:	x, y - 2D position
iPXSize - printed text size
sText - text to print

Result:	Prints formatted text at specified position
with specified pixel size.

/*---------------------------------------------*/

void CFreeTypeFont::PrintFormatted(int x, int y, int iPXSize, char* sText, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, sText);
	vsprintf(buf, sText, ap);
	va_end(ap);
	Print(buf, x, y, iPXSize);
}

/*-----------------------------------------------

Name:	releaseFont

Params:	none

Result:	Deletes all font textures.

/*---------------------------------------------*/

void CFreeTypeFont::DeleteFont()
{
	for (int i = 0; i < 128; i++)m_tCharTextures[i].DeleteTexture();
	m_vboData.DeleteVBO();
	glDeleteVertexArrays(1, &m_uiVAO);
}

/*-----------------------------------------------

Name:	setShaderProgram

Params:	a_shShaderProgram - shader program

Result:	Sets shader program that font uses.

/*---------------------------------------------*/

void CFreeTypeFont::SetShaderProgram(CShaderProgram* a_shShaderProgram)
{
	m_shShaderProgram = a_shShaderProgram;
}
