#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"

glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(0, 0, 0);
Model model;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0, 0.0, -1.0f), glm::vec3(0.0, 1.0, 0.0));
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/skybox.vs", "Res/skybox.fs");
	model.mShader->SetTextureCube("U_Texture", CreateTextureCubeFromBMP("Res/front.bmp", "Res/back.bmp",
		"Res/left.bmp", "Res/right.bmp", "Res/bottom.bmp", "Res/top.bmp"));
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(1, 1, 1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}