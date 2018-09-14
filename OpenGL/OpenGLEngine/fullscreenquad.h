#pragma once
#include "ggl.h"
#include "vertexbuffer.h"
#include "shader.h"
class FullScreenQuad{
public:
	VertexBuffer *mVertexBuffer;
	Shader *mShader;
public:
	void Init();
	void Draw();
	void DrawToLeftTop();
	void DrawToRightTop();
	void DrawToLeftBottom();
	void DrawToRightBottom();
};
