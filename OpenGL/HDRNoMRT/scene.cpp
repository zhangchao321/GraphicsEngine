#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO, *ldrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV;
Shader * ldrShader;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/direction_light.vs", "Res/direction_light_hdr1.fs");
	model.SetAmbientMaterial(0.0, 0.0, 0.0, 1);
	model.mShader->SetVec4("U_AmbientLight", 0.0, 0.0, 0.0, 1.0);
	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.SetSpecularMaterial(1, 1, 1, 1);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	
	ldrShader = new Shader;
	ldrShader->Init("Res/direction_light.vs", "Res/direction_light_ldr.fs");
	ldrShader->SetVec4("U_AmbientMaterial", 0.0, 0.0, 0.0, 1.0);
	ldrShader->SetVec4("U_AmbientLight", 0.0, 0.0, 0.0, 1.0);
	ldrShader->SetVec4("U_DiffuseMaterial", 0.4, 0.4, 0.4, 1.0);
	ldrShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	ldrShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	ldrShader->SetVec4("U_SpecularMaterial", 1, 1, 1, 1);
	ldrShader->SetVec4("U_LightPos", 0, 1, 0, 0);
	ldrShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);

	
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
	fsq->mShader->SetTexture("U_Texture", blurfboV->GetBuffer("color"));

	hdrFBO = new FrameBufferObject;
	hdrFBO->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	hdrFBO->AttachDepthBuffer("depth", (int)width, (int)height);
	hdrFBO->Finish();

	ldrFBO = new FrameBufferObject;
	ldrFBO->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	ldrFBO->AttachDepthBuffer("depth", (int)width, (int)height);
	ldrFBO->Finish();
}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hdrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	hdrFBO->Unbind();

	Shader *tempShader = model.mShader;
	model.mShader = ldrShader;
	ldrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	ldrFBO->Unbind();

	fsq->mShader->SetTexture("U_Texture", ldrFBO->GetBuffer("color"));
	fsq->DrawToRightTop();

	model.mShader = tempShader;
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	fsq->DrawToLeftTop();
	
}