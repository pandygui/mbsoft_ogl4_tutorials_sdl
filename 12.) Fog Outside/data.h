#pragma once

#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"

// Shader data

#define NUMSHADERS 9
extern CShader shShaders[NUMSHADERS];
extern CShaderProgram spFogAndLight, spOrtho2D, spFont2D;

void LoadShaders();

// Texture data

#define NUMTEXTURES 3
extern CTexture tTextures[NUMTEXTURES];

void LoadTextures();