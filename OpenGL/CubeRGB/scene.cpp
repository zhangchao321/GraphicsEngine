#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 3.0f);
Model model;
FrameBufferObject * fbo =nullptr;
FullScreenQuad * fsq =nullptr;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0,1.0,0.0));
	model.Init("Res/Cube.obj");
	model.SetPosition(0.0, 0.0, 0.0);
	model.mShader->Init("Res/rgbcube.vs", "Res/rgbcube.fs");

	fsq = new FullScreenQuad;
	fsq->Init();
	fsq->mShader->Init("Res/fullscreenquad.vs", "Res/gray.fs");
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);

	fbo = new FrameBufferObject;
	fbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	fbo->AttachDepthBuffer("depth", (int)width, (int)height);
	fbo->Finish();
	fsq->mShader->SetTexture("U_Texture", fbo->GetBuffer("color"));
	fbo->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	fbo->Unbind();
}

void Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fsq->Draw();
}
