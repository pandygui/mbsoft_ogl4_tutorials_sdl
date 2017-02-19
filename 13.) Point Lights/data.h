#pragma once

#include "../common_classes/shaders.h"
#include "../common_classes/texture.h"

// Shader data

#define NUMSHADERS 10
extern CShader shShaders[NUMSHADERS];
extern CShaderProgram spPointLights, spOrtho2D, spFont2D, spColor;

void LoadShaders();

// Texture data

#define NUMTEXTURES 4
extern CTexture tTextures[NUMTEXTURES];

void LoadTextures();