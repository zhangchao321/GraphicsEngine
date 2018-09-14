#include "fullscreenquad.h"
void FullScreenQuad::Init(){
	mVertexBuffer = new VertexBuffer;
	mVertexBuffer->SetSize(4);
	mVertexBuffer->SetTexcoord(0, 0.0f, 0.0f);
	mVertexBuffer->SetTexcoord(1, 1.0f, 0.0f);
	mVertexBuffer->SetTexcoord(2, 0.0f, 1.0f);
	mVertexBuffer->SetTexcoord(3, 1.0f, 1.0f);
	mShader = new Shader;
}
void FullScreenQuad::Draw(){
	float identity[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	mVertexBuffer->SetPosition(0, -1.0f, -1.0f, 0.0f);
	mVertexBuffer->SetPosition(1, 1.0f, -1.0f, 0.0f);
	mVertexBuffer->SetPosition(2, -1.0f, 1.0f, 0.0f);
	mVertexBuffer->SetPosition(3, 1.0f, 1.0f, 0.0f);
	mVertexBuffer->Bind();
	mShader->Bind(identity, identity, identity);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	mVertexBuffer->Unbind();
}
void FullScreenQuad::DrawToLeftTop(){
	float identity[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	mVertexBuffer->SetPosition(0, -1.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(1, 0.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(2, -1.0f, 1.0f, -1.0f);
	mVertexBuffer->SetPosition(3, 0.0f, 1.0f, -1.0f);
	mVertexBuffer->Bind();
	mShader->Bind(identity, identity, identity);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	mVertexBuffer->Unbind();
}
void FullScreenQuad::DrawToLeftBottom(){
	float identity[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	mVertexBuffer->SetPosition(0, -1.0f, -1.0f, -1.0f);
	mVertexBuffer->SetPosition(1, 0.0f, -1.0f, -1.0f);
	mVertexBuffer->SetPosition(2, -1.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(3, 0.0f, 0.0f, -1.0f);
	mVertexBuffer->Bind();
	mShader->Bind(identity, identity, identity);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	mVertexBuffer->Unbind();
}
void FullScreenQuad::DrawToRightTop(){
	float identity[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	mVertexBuffer->SetPosition(0, 0.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(1, 1.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(2, 0.0f, 1.0f, -1.0f);
	mVertexBuffer->SetPosition(3, 1.0f, 1.0f, -1.0f);
	mVertexBuffer->Bind();
	mShader->Bind(identity, identity, identity);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	mVertexBuffer->Unbind();
}
void FullScreenQuad::DrawToRightBottom(){
	float identity[] = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	mVertexBuffer->SetPosition(0, 0.0f, -1.0f, -1.0f);
	mVertexBuffer->SetPosition(1, 1.0f, -1.0f, -1.0f);
	mVertexBuffer->SetPosition(2, 0.0f, 0.0f, -1.0f);
	mVertexBuffer->SetPosition(3, 1.0f, 0.0f, -1.0f);
	mVertexBuffer->Bind();
	mShader->Bind(identity, identity, identity);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	mVertexBuffer->Unbind();
}
