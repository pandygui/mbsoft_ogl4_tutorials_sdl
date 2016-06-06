#include "common_header.h"

#include "vertexBufferObject.h"

CVertexBufferObject::CVertexBufferObject()
{
	m_bDataUploaded = false;
	m_uiBuffer = 0;
}

/*-----------------------------------------------

Name:	CreateVBO

Params:	a_iSize - initial size of buffer

Result:	Creates vertex buffer object.

/*---------------------------------------------*/

void CVertexBufferObject::CreateVBO(int iSize)
{
	glGenBuffers(1, &m_uiBuffer);
	data.reserve(iSize);
}

/*-----------------------------------------------

Name:	DeleteVBO

Params:	none

Result:	Releases VBO and frees all memory.

/*---------------------------------------------*/

void CVertexBufferObject::DeleteVBO()
{
	glDeleteBuffers(1, &m_uiBuffer);
	m_bDataUploaded = false;
	data.clear();
}

/*-----------------------------------------------

Name:	mapBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...

Result:	Maps whole buffer data to memory and
		returns pointer to data.

/*---------------------------------------------*/

void* CVertexBufferObject::MapBufferToMemory(int iUsageHint)
{
	if(!m_bDataUploaded)
		return NULL;

	void* ptrRes = glMapBuffer(m_iBufferType, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------

Name:	MapSubBufferToMemory

Params:	iUsageHint - GL_READ_ONLY, GL_WRITE_ONLY...
		uiOffset - data offset (from where should
					data be mapped).
		uiLength - length of data

Result:	Maps specified part of buffer to memory.

/*---------------------------------------------*/

void* CVertexBufferObject::MapSubBufferToMemory(int iUsageHint, unsigned int uiOffset, unsigned int uiLength)
{
	if(!m_bDataUploaded)
		return NULL;

	void* ptrRes = glMapBufferRange(m_iBufferType, uiOffset, uiLength, iUsageHint);
	return ptrRes;
}

/*-----------------------------------------------

Name:	UnmapBuffer

Params:	none

Result:	Unmaps previously mapped buffer.

/*---------------------------------------------*/

void CVertexBufferObject::UnmapBuffer()
{
	glUnmapBuffer(m_iBufferType);
}

/*-----------------------------------------------

Name:	BindVBO

Params:	a_iBufferType - buffer type (GL_ARRAY_BUFFER, ...)

Result:	Binds this VBO.

/*---------------------------------------------*/

void CVertexBufferObject::BindVBO(int iBufferType)
{
	m_iBufferType = iBufferType;
	glBindBuffer(iBufferType, m_uiBuffer);
}

/*-----------------------------------------------

Name:	UploadDataToGPU

Params:	iUsageHint - GL_STATIC_DRAW, GL_DYNAMIC_DRAW...

Result:	Sends data to GPU.

/*---------------------------------------------*/

void CVertexBufferObject::UploadDataToGPU(int iDrawingHint)
{
	glBufferData(m_iBufferType, data.size(), &data[0], iDrawingHint);
	m_bDataUploaded = true;
	data.clear();
}

/*-----------------------------------------------

Name:	AddData

Params:	ptrData - pointer to arbitrary data
		uiDataSize - data size in bytes

Result:	Adds arbitrary data to VBO.

/*---------------------------------------------*/

void CVertexBufferObject::AddData(void* ptrData, unsigned int uiDataSize)
{
	data.insert(data.end(), (unsigned char*)ptrData, (unsigned char*)ptrData+uiDataSize);
}

/*-----------------------------------------------

Name:	GetDataPointer

Params:	none

Result:	Returns data pointer (only before uploading).

/*---------------------------------------------*/

void* CVertexBufferObject::GetDataPointer()
{
	if(m_bDataUploaded)
		return NULL;

	return (void*)data[0];
}

/*-----------------------------------------------

Name:	GetBufferID

Params:	none

Result:	Returns VBO ID.

/*---------------------------------------------*/

GLuint CVertexBufferObject::GetBufferID()
{
	return m_uiBuffer;
}

/*-----------------------------------------------

Name:	GetCurrentSize

Params:	none

Result: Returns size of data that has been added to
		the buffer object.

/*---------------------------------------------*/

unsigned int CVertexBufferObject::GetCurrentSize()
{
	return (unsigned int)data.size();
}