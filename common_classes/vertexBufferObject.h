#pragma once

class CVertexBufferObject
{
public:
	CVertexBufferObject();

	void CreateVBO(int iSize = 0);
	void DeleteVBO();

	void* MapBufferToMemory(int iUsageHint);
	void* MapSubBufferToMemory(int iUsageHint, unsigned int uiOffset, unsigned int uiLength);
	void UnmapBuffer();

	void BindVBO(int iBufferType = GL_ARRAY_BUFFER);
	void UploadDataToGPU(int iUsageHint);
	
	void AddData(void* ptrData, unsigned int uiDataSize);

	void* GetDataPointer();
	GLuint GetBufferID();

	unsigned int GetCurrentSize();

private:
	GLuint m_uiBuffer;
	int m_iBufferType;
	vector<unsigned char> data;

	bool m_bDataUploaded;
};