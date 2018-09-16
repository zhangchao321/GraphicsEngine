#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 3.0f);
Model model;
FrameBufferObject * fbo = nullptr;
FullScreenQuad * fsq = nullptr;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/ambient_vs.vs", "Res/ambient_vs.fs");
	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}