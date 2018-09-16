#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo, *blurfboH,*blurfboV;
FullScreenQuad *fsq, *blurFSQH,*blurFSQV;
void Init() {
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/rgbcube.vs", "Res/rgbcube.fs");
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	fsq = new FullScreenQuad;
	fsq->Init();
	fsq->mShader->Init("Res/fullscreenquad.vs", "Res/fullscreenquad.fs");

	blurFSQH = new FullScreenQuad;
	blurFSQH->Init();
	blurFSQH->mShader->Init("Res/fullscreenquad.vs", "Res/GaussianHorizontal.fs");

	blurFSQV = new FullScreenQuad;
	blurFSQV->Init();
	blurFSQV->mShader->Init("Res/fullscreenquad.vs", "Res/GaussianVertical.fs");
}
void SetViewPort(float width, float height) {
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.0f);
	fbo = new FrameBufferObject;
	fbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	fbo->AttachDepthBuffer("depth", (int)width, (int)height);
	fbo->Finish();
	blurFSQH->mShader->SetTexture("U_Texture", fbo->GetBuffer("color"));

	blurfboH = new FrameBufferObject;
	blurfboH->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	blurfboH->AttachDepthBuffer("depth", (int)width, (int)height);
	blurfboH->Finish();
	blurFSQV->mShader->SetTexture("U_Texture", blurfboH->GetBuffer("color"));

	blurfboV = new FrameBufferObject;
	blurfboV->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	blurfboV->AttachDepthBuffer("depth", (int)width, (int)height);
	blurfboV->Finish();
	fsq->mShader->SetTexture("U_Texture", blurfboH->GetBuffer("color"));
}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbo->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	fbo->Unbind();

	blurfboH->Bind();
	blurFSQH->Draw();
	blurfboH->Unbind();

	blurfboV->Bind();
	blurFSQV->Draw();
	blurfboV->Unbind();

	fsq->Draw();
}