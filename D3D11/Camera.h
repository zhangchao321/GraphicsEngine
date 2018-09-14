#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMMATRIX GetViewMatrixXM() const;
	XMVECTOR   GetPositionXM() const;
	XMVECTOR   GetRotationXM() const;

	XMFLOAT4X4 GetViewMatrix() const;
	XMFLOAT3   GetPosition() const;
	XMFLOAT3   GetRotation() const;

	// Generate view matrix.
	void Render();

private:
	XMFLOAT3   mPos;
	XMFLOAT3   mRot;
	XMFLOAT4X4 mView;
};

#endif
