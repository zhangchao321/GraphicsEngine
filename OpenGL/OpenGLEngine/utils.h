#pragma once
#include "ggl.h"

unsigned char* LoadFileContent(const char * path, int &fileSize);
unsigned char* DecodeBMP(unsigned char*bmpFileData, int&width, int&height);
GLuint CreateTexture2D(unsigned char*pixelData, int width, int height, GLenum type);
GLuint CreateTexture2DFromBMP(const char*bmpPath);
GLuint CreateTextureCubeFromBMP(const char *front, const char *back, const char *left,
	const char *right, const char *top, const char *bottom);