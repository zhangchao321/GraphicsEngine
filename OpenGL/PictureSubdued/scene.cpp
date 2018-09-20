#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
Model lightModel;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV, *combineFSQ, *imgFSQ;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/direction_light.vs", "Res/direction_light_hdr.fs");
	model.SetAmbientMaterial(0.0, 0.0, 0.0, 1);
	model.mShader->SetVec4("U_AmbientLight", 0.0, 0.0, 0.0, 1.0);
	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.SetSpecularMaterial(1, 1, 1, 1);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	lightModel.Init("Res/Sphere.obj");
	lightModel.mShader->Init("Res/light_renderer.vs", "Res/light_renderer.fs");
	lightModel.mModelMatrix = glm::translate(glm::mat4(1.0),glm::vec3(0.0, 1.2, 0.0))*glm::scale(glm::mat4(1.0),glm::vec3(0.1, 0.1, 0.1));

	fsq = new FullScreenQuad;
	fsq->Init();
	fsq->mShader->Init("Res/fullscreenquad.vs", "Res/fullscreenquad.fs");

	combineFSQ = new FullScreenQuad;
	combineFSQ->Init();
	combineFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/hdrcombine.fs");

	blurFSQH = new FullScreenQuad;
	blurFSQH->Init();
	blurFSQH->mShader->Init("Res/fullscreenquad.vs", "Res/GaussianHorizontal.fs");

	blurFSQV = new FullScreenQuad;
	blurFSQV->Init();
	blurFSQV->mShader->Init("Res/fullscreenquad.vs", "Res/GaussianVertical.fs");

	imgFSQ = new FullScreenQuad;
	imgFSQ->Init();
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/rg.fs");
	imgFSQ->mShader->SetTexture("U_Texture1", "Res/stone.bmp");
	imgFSQ->mShader->SetTexture("U_Texture2", "Res/wood.bmp");
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
	fsq->mShader->SetTexture("U_Texture", blurfboV->GetBuffer("color"));

	hdrFBO = new FrameBufferObject;
	hdrFBO->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	hdrFBO->AttachColorBuffer("hdr", GL_COLOR_ATTACHMENT1, (int)width, (int)height);
	hdrFBO->AttachDepthBuffer("depth", (int)width, (int)height);
	hdrFBO->Finish();

}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	imgFSQ->Draw();
}