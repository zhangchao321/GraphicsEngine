#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo;
FullScreenQuad *fsq;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/reflection.vs", "Res/reflection.fs");
	model.mShader->SetTextureCube("U_Texture", CreateTextureCubeFromBMP("Res/front.bmp", "Res/back.bmp",
		"Res/left.bmp", "Res/right.bmp", "Res/bottom.bmp", "Res/top.bmp"));
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}
void SetViewPort(float width, float height) {
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.0f);
}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}