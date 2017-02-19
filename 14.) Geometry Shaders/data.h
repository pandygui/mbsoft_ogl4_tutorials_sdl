#pragma once

#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"

// Shader data

#define NUMSHADERS 6
extern CShader shShaders[NUMSHADERS];
extern CShaderProgram spMain, spFont2D;

void LoadShaders();

// Texture data

#define NUMTEXTURES 4
extern CTexture tTextures[NUMTEXTURES];

void LoadTextures();