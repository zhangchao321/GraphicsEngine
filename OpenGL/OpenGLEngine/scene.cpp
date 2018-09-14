//CubeRGB
/*#include "scene.h"
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
}*/

//Material Ambient_vs
/*#include "scene.h"
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
}*/

//Material diffuse_vs
/*#include "scene.h"
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
	model.mShader->Init("Res/specular_vs.vs", "Res/specular_vs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0,1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5f, 0.5f, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Material Blin_vs
/*#include "scene.h"
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
	model.mShader->Init("Res/blin_vs.vs", "Res/blin_vs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Material ambient_fs
/*#include "scene.h"
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
	model.mShader->Init("Res/ambient_fs.vs", "Res/ambient_fs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5f, 0.5f, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Material diffuse_fs
/*#include "scene.h"
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
	model.mShader->Init("Res/diffuse_fs.vs", "Res/diffuse_fs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Material Specular_fs
/*#include "scene.h"
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
	model.mShader->Init("Res/specular_fs.vs", "Res/specular_fs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/


//Material Blin_fs
/*#include "scene.h"
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
	model.mShader->Init("Res/blin_fs.vs", "Res/blin_fs.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/


//Direction Light
/*#include "scene.h"
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
	model.mShader->Init("Res/direction_light.vs", "Res/direction_light.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/


//Point Light
/*#include "scene.h"
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
	model.mShader->Init("Res/point_light.vs", "Res/point_light.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 2, 0, 1);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/


//Spot Light
/*#include "scene.h"
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
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/spot_light.vs", "Res/spot_light.fs");

	model.SetPosition(0.0, 0.0, 0.0);
	model.SetAmbientMaterial(0.1, 0.1, 0.1, 1.0);
	model.mShader->SetVec4("U_AmbientLight", 0.1, 0.1, 0.1, 1.0);

	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight", 0.8, 0.8, 0.8, 1.0);
	model.mShader->SetVec4("U_LightPos", 0, 1.5, 0, 1);

	model.SetSpecularMaterial(1.0, 1.0, 1.0, 1.0);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);

	model.mShader->SetVec4("U_LightDirection", 0, -1, 0, 15);
	model.mShader->SetVec4("U_LightOption", 32, 2, 0, 0);

	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.5, 0.5, 0.5, 0.1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/


//Fog Linear
/*#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(2, 1, 2);
Model model;
FrameBufferObject * fbo = nullptr;
FullScreenQuad * fsq = nullptr;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/fog.vs", "Res/fog.fs");
	model.mShader->SetVec4("U_FogOption", 0.1, 3.0, 0, 0);
	model.mShader->SetVec4("U_FogColor", 0.8, 0.8, 0.8, 1.0);

	model.SetPosition(0.0, 0.0, 0.0);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Fog Exp
/*#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(2, 1, 2);
Model model;
FrameBufferObject * fbo = nullptr;
FullScreenQuad * fsq = nullptr;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/fog.vs", "Res/fogexp.fs");
	model.mShader->SetVec4("U_FogOption", 0.1, 3.0, 1.0, 0);
	model.mShader->SetVec4("U_FogColor", 0.8, 0.8, 0.8, 1.0);

	model.SetPosition(0.0, 0.0, 0.0);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//Fog Expx
/*#include "scene.h"
#include"utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(2, 1, 2);
Model model;
FrameBufferObject * fbo = nullptr;
FullScreenQuad * fsq = nullptr;
void Init()
{
	viewMatrix = glm::lookAt(cameraPos,
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
	model.Init("Res/Cube.obj");
	model.mShader->Init("Res/fog.vs", "Res/fogexpx.fs");
	model.mShader->SetVec4("U_FogOption", 0.1, 3.0, 1.0, 0.5);
	model.mShader->SetVec4("U_FogColor", 0.8, 0.8, 0.8, 1.0);

	model.SetPosition(0.0, 0.0, 0.0);
}

void SetViewPort(float width, float height)
{
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.f);
}

void Draw()
{
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
}*/

//skybox
/*#include "scene.h"
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
}*/

//reflection
/*#include "scene.h"
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
}*/

//refraction
/*#include "scene.h"
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
	model.mShader->Init("Res/refraction.vs", "Res/refraction.fs");
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
}*/

//Gaussian
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo,*blurfbo;
FullScreenQuad *fsq,*blurFSQ;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/reflection.vs", "Res/reflection.fs");
	model.mShader->SetTextureCube("U_Texture", CreateTextureCubeFromBMP("Res/front.bmp", "Res/back.bmp",
		"Res/left.bmp", "Res/right.bmp", "Res/bottom.bmp", "Res/top.bmp"));
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	fsq = new FullScreenQuad;
	fsq->Init();
	fsq->mShader->Init("Res/fullscreenquad.vs", "Res/fullscreenquad.fs");

	blurFSQ = new FullScreenQuad;
	blurFSQ->Init();
	blurFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/Gaussian.fs");
}
void SetViewPort(float width, float height) {
	projectionMatrix = glm::perspective(50.0f, width / height, 0.1f, 100.0f);
	fbo = new FrameBufferObject;
	fbo->AttachColorBuffer("color",GL_COLOR_ATTACHMENT0,(int)width,(int)height);
	fbo->AttachDepthBuffer("depth", (int)width, (int)height);
	fbo->Finish();
	blurFSQ->mShader->SetTexture("U_Texture", fbo->GetBuffer("color"));

	blurfbo = new FrameBufferObject;
	blurfbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, (int)width, (int)height);
	blurfbo->AttachDepthBuffer("depth", (int)width, (int)height);
	blurfbo->Finish();
	fsq->mShader->SetTexture("U_Texture", blurfbo->GetBuffer("color"));

}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	fbo->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	fbo->Unbind();

	blurfbo->Bind();
	blurFSQ->Draw();
	blurfbo->Unbind();

	fsq->Draw();
}*/

//Gaussian HV
/*#include "scene.h"
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
}*/

//Gaussian HV Mutil
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo, *blurfboH, *blurfboV;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/reflection.vs", "Res/reflection.fs");
	model.mShader->SetTextureCube("U_Texture", CreateTextureCubeFromBMP("Res/front.bmp", "Res/back.bmp",
		"Res/left.bmp", "Res/right.bmp", "Res/bottom.bmp", "Res/top.bmp"));
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
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
	fsq->DrawToLeftTop();

	blurFSQH->mShader->SetTexture("U_Texture", blurfboV->GetBuffer("color"));
	blurfboH->Bind();
	blurFSQH->Draw();
	blurfboH->Unbind();
	blurfboV->Bind();
	blurFSQV->Draw();
	blurfboV->Unbind(); 
	fsq->DrawToLeftBottom();
}*/

// HDR MRT
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo, *blurfboH, *blurfboV,*hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV;
void Init() {
	model.Init("Res/Sphere.obj");
	model.mShader->Init("Res/direction_light.vs", "Res/direction_light_hdr.fs");
	model.SetAmbientMaterial(0.0, 0.0, 0.0, 1);
	model.mShader->SetVec4("U_AmbientLight", 0.0, 0.0, 0.0, 1.0);
	model.SetDiffuseMaterial(0.4, 0.4, 0.4, 1.0);
	model.mShader->SetVec4("U_DiffuseLight",0.8,0.8,0.8,1.0);
	model.SetSpecularMaterial(1, 1, 1, 1);
	model.mShader->SetVec4("U_SpecularLight", 1, 1, 1, 1);
	model.mShader->SetVec4("U_LightPos", 0, 1, 0, 0);
	model.mShader->SetVec4("U_CameraPos", cameraPos.x, cameraPos.y, cameraPos.z, 1.0f);
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
	hdrFBO->AttachColorBuffer("color",GL_COLOR_ATTACHMENT0,(int)width,(int)height);
	hdrFBO->AttachColorBuffer("hdr", GL_COLOR_ATTACHMENT1, (int)width, (int)height);
	hdrFBO->AttachDepthBuffer("depth", (int)width, (int)height);
	hdrFBO->Finish();

}
void Draw() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	hdrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	hdrFBO->Unbind();
	fsq->mShader->SetTexture("U_Texture",hdrFBO->GetBuffer("color"));
	fsq->DrawToLeftTop();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("hdr"));
	fsq->DrawToRightTop();
}*/

//HDR No MRT
/*#include "scene.h"
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
	
}*/

//Combine Hdr
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV,*combineFSQ;
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
	hdrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	hdrFBO->Unbind();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	fsq->DrawToLeftTop();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("hdr"));
	fsq->DrawToRightTop();
	combineFSQ->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	combineFSQ->mShader->SetTexture("U_HDRTexture", hdrFBO->GetBuffer("hdr"));
	combineFSQ->DrawToLeftBottom();
}*/


//Hdr Light
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
Model lightModel;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV, *combineFSQ;
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	hdrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	lightModel.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	hdrFBO->Unbind();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	fsq->DrawToLeftTop();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("hdr"));
	fsq->DrawToRightTop();
	combineFSQ->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	combineFSQ->mShader->SetTexture("U_HDRTexture", hdrFBO->GetBuffer("hdr"));
	combineFSQ->DrawToLeftBottom();
}*/


//blur Hdr Light
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
Model lightModel;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV, *combineFSQ;
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	hdrFBO->Bind();
	model.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	lightModel.Draw(viewMatrix, projectionMatrix, cameraPos.x, cameraPos.y, cameraPos.z);
	hdrFBO->Unbind();

	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	fsq->DrawToLeftTop();
	fsq->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("hdr"));
	fsq->DrawToRightTop();

	 
	blurfboH->Bind();
	blurFSQH->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("hdr"));
	blurFSQH->Draw();
	blurfboH->Unbind(); 

	blurfboV->Bind();
	blurFSQV->mShader->SetTexture("U_Texture", blurfboH->GetBuffer("color"));	
	blurFSQV->Draw();
	blurfboV->Unbind();
	fsq->mShader->SetTexture("U_Texture", blurfboV->GetBuffer("color"));
	fsq->DrawToLeftBottom();

	combineFSQ->mShader->SetTexture("U_Texture", hdrFBO->GetBuffer("color"));
	combineFSQ->mShader->SetTexture("U_HDRTexture", blurfboV->GetBuffer("color"));
	combineFSQ->DrawToRightBottom();
}*/

//blend image 
/*#include "scene.h"
#include "utils.h"
#include "model.h"
#include "framebufferobject.h"
#include "fullscreenquad.h"
glm::mat4 viewMatrix, projectionMatrix;
glm::vec3 cameraPos(4.0f, 3.0f, 4.0f);
Model model;
Model lightModel;
FrameBufferObject *fbo, *blurfboH, *blurfboV, *hdrFBO;
FullScreenQuad *fsq, *blurFSQH, *blurFSQV, *combineFSQ,*imgFSQ;
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs","Res/alpha_blend.fs");
	imgFSQ->mShader->SetTexture("U_Texture1", "Res/stone.bmp");
	imgFSQ->mShader->SetTexture("U_Texture2","Res/wood.bmp");
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
}*/

//lighter
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/lighter.fs");
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
}*/

//darker
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/darker.fs");
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
}*/

//ÕýÆ¬µþµ×
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/zpdd.fs");
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
}*/

//moto_darker
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/moto_darker.fs");
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
}*/

//moto_lighter
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/moto_lighter.fs");
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
}*/

//Èá¹â
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
}*/

//add
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/add.fs");
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
}*/


//Minus
/*#include "scene.h"
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/minus.fs");
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
}*/

//µþ¼Ó
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
	lightModel.mModelMatrix = glm::translate(0.0, 1.2, 0.0)*glm::scale(0.1, 0.1, 0.1);

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
	imgFSQ->mShader->Init("Res/fullscreenquad.vs", "Res/dj.fs");
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