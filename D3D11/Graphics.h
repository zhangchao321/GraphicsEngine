#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <windows.h>
#include "D3D.h"
#include "Text.h"
#include "Model.h"
#include "ModelList.h"
#include "Timer.h"
#include "Light.h"
#include "Camera.h"
#include "Frustum.h"
#include "Shader.h"

const bool GRAPHICS_FULL_SCREEN = false;
const bool GRAPHICS_VSYNC_ENABLE = true;
const float GRAPHICS_SCREEN_DEPTH = 1000.0f;
const float GRAPHICS_SCREEN_NEAR = 1.0f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics& other);
	~Graphics();

	bool Init(HWND hwnd, int width, int height);
	bool Frame(float dt, int fps, int cpuRate, float rotY);
	bool Render();
	void Shutdown();

private:
	D3D*    mD3D;
	Timer*  mTimer;
	Light*  mLight;
	Camera* mCamera;
	Frustum* mFrustum;
	Shader* mShader;

	Text*   mText;
	Model*  mModel;
	ModelList* mModelList;
};
#endif
