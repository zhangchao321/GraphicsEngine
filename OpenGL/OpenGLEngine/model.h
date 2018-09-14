#pragma once
#include "ggl.h"
#include "vertexbuffer.h"
#include "shader.h"
class Model {
public:
	VertexBuffer*mVertexBuffer;
	Shader*mShader;
public:
	glm::mat4 mModelMatrix;
	float *mLightViewMatrix, *mLightProjectionMatrix;
	Model();
	void Init(const char*modelPath);
	void Draw(glm::mat4 & viewMatrix, glm::mat4 projectionMatrix, float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetAmbientMaterial(float r, float g, float b, float a);
	void SetDiffuseMaterial(float r, float g, float b, float a);
	void SetSpecularMaterial(float r, float g, float b, float a);
	void SetTexture(const char*imagePath);
};