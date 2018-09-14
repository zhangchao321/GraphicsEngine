#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include <DirectXMath.h>
using namespace DirectX;

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum& other);
	~Frustum();

	void ConstructFrustum(float screenDepth, XMFLOAT4X4 proj, XMFLOAT4X4 view);

	bool CheckPoint(XMFLOAT3 point);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);
		
private:
	XMFLOAT4 mPlanes[6];
};

#endif