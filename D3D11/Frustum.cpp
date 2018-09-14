#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum& other)
{
}

Frustum::~Frustum()
{
}

void Frustum::ConstructFrustum(float screenDepth, XMFLOAT4X4 proj, XMFLOAT4X4 view)
{
	// Update proj matrix's far plane = screenDepth
	// nz means near plane's depth of frustum
	float nz = -(proj(4, 3) / proj(3, 3));
	float r = screenDepth / (screenDepth - nz);
	proj(3, 3) = r;
	proj(4, 3) = -r*nz;

	// frustum matrix
	XMMATRIX f = XMLoadFloat4x4(&view) * XMLoadFloat4x4(&proj);
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, f);

	// near plane
	mPlanes[0].x = M(1, 4) + M(1, 3);
	mPlanes[0].y = M(2, 4) + M(2, 3);
	mPlanes[0].z = M(3, 4) + M(3, 3);
	mPlanes[0].w = M(4, 4) + M(4, 3);

	// far plane
	mPlanes[1].x = M(1, 4) - M(1, 3);
	mPlanes[1].y = M(2, 4) - M(2, 3);
	mPlanes[1].z = M(3, 4) - M(3, 3);
	mPlanes[1].w = M(4, 4) - M(4, 3);

	// left plane
	mPlanes[2].x = M(1, 4) + M(1, 1);
	mPlanes[2].y = M(2, 4) + M(2, 1);
	mPlanes[2].z = M(3, 4) + M(3, 1);
	mPlanes[2].w = M(4, 4) + M(4, 1);

	// right plane
	mPlanes[3].x = M(1, 4) - M(1, 1);
	mPlanes[3].y = M(2, 4) - M(2, 1);
	mPlanes[3].z = M(3, 4) - M(3, 1);
	mPlanes[3].w = M(4, 4) - M(4, 1);

	// top plane
	mPlanes[4].x = M(1, 4) - M(1, 2);
	mPlanes[4].y = M(2, 4) - M(2, 2);
	mPlanes[4].z = M(3, 4) - M(3, 2);
	mPlanes[4].w = M(4, 4) - M(4, 2);

	// bottom plane
	mPlanes[5].x = M(1, 4) + M(1, 2);
	mPlanes[5].y = M(2, 4) + M(2, 2);
	mPlanes[5].z = M(3, 4) + M(3, 2);
	mPlanes[5].w = M(4, 4) + M(4, 2);

	// Normalize the plane equations.
	for (int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&mPlanes[i]));
		XMStoreFloat4(&mPlanes[i], v);
	}
}

bool Frustum::CheckPoint(XMFLOAT3 point)
{
	XMVECTOR v = XMLoadFloat3(&point);
	for (int i = 0; i < 6; ++i)
	{
		float dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot < 0.0f)
			return false;
	}
	return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	XMFLOAT3 tmp;
	XMVECTOR v;
	float dot;
	for (int i = 0; i < 6; ++i)
	{
		tmp = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter - radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter - radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter - radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter - radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter - radius, zCenter + radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter - radius, zCenter + radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - radius, yCenter + radius, zCenter + radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + radius, yCenter + radius, zCenter + radius);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		return false;
	}
	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	XMFLOAT3 tmp = XMFLOAT3(xCenter, yCenter, zCenter);
	XMVECTOR v;
	v = XMLoadFloat3(&tmp);
	for (int i = 0; i < 6; ++i)
	{
		float dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot + radius < 0.0f)
			return false;
	}
	return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	XMFLOAT3 tmp;
	XMVECTOR v;
	float dot;
	for (int i = 0; i < 6; ++i)
	{
		tmp = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter - zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter - zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter - zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter - zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter - ySize, zCenter + zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter - ySize, zCenter + zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter - xSize, yCenter + ySize, zCenter + zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		tmp = XMFLOAT3(xCenter + xSize, yCenter + ySize, zCenter + zSize);
		v = XMLoadFloat3(&tmp);
		dot = XMPlaneDotCoord(XMLoadFloat4(&mPlanes[i]), v).m128_f32[0];
		if (dot >= 0.0f)
			continue;

		return false;
	}
	return true;
}
