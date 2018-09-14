#include "Camera.h"

Camera::Camera() : mPos(0.0f, 0.0f, 0.0f), mRot(0.0f, 0.0f, 0.0f)
{
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	mPos = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float x, float y, float z)
{
	mRot = XMFLOAT3(x, y, z);
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPos);
}

XMVECTOR Camera::GetRotationXM() const
{
	return XMLoadFloat3(&mRot);
}

XMMATRIX Camera::GetViewMatrixXM() const
{
	return XMLoadFloat4x4(&mView);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPos;
}

XMFLOAT3 Camera::GetRotation() const
{
	return mRot;
}

XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return mView;
}


void Camera::Render()
{
	XMFLOAT4 pos(mPos.x, mPos.y, mPos.z, 0.0f);
	XMFLOAT4 up(0.0f, 1.0f, 0.0f, 0.0f);
	XMFLOAT4 lookAt(0.0f, 0.0f, 1.0f, 0.0f);

	XMVECTOR vPos = XMLoadFloat4(&pos);
	XMVECTOR vUp = XMLoadFloat4(&up);
	XMVECTOR vLookAt = XMLoadFloat4(&lookAt);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	// 0.0174532925f is same to PI/180
	float angleToRadian = XM_PI / 180;
	float pitch = mRot.x * angleToRadian;
	float yaw = mRot.y * angleToRadian;
	float roll = mRot.z * angleToRadian;
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	vUp = XMVector3TransformCoord(vUp, rotationMatrix);
	vLookAt = XMVector3TransformCoord(vLookAt, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	vLookAt = vPos + vLookAt;
	XMMATRIX viewMatrix = XMMatrixLookAtLH(vPos, vLookAt, vUp);
	XMStoreFloat4x4(&mView, viewMatrix);
}