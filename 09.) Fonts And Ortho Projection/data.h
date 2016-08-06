#pragma once

#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"

// Shader data

#define NUMSHADERS 5
extern CShader shShaders[NUMSHADERS];
extern CShaderProgram spDirectionalLight, spOrtho2D, spFont2D;

void LoadShaders();

// Texture data

#define NUMTEXTURES 1
extern CTexture tTextures[NUMTEXTURES];

void LoadTextures();