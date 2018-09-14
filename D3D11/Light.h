#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <DirectXMath.h>
using namespace DirectX;

class Light
{
public:
	Light();
	Light(const Light& other);
	~Light();

	void SetAmbientColor(float r, float g, float b, float a);
	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDiffuseDir(float x, float y, float z);
	void SetSpecularPower(float power);
	void SetSpecularColor(float r, float g, float b, float a);

	XMFLOAT4 GetAmbientColor() const;
	XMFLOAT4 GetDiffuseColor() const;
	XMFLOAT3 GetDiffuseDir() const;
	float    GetSpecularPower() const;
	XMFLOAT4 GetSpecularColor() const;

private:
	XMFLOAT4 mAmbientColor;
	XMFLOAT4 mDiffuseColor;
	XMFLOAT3 mDiffuseDir;
	float    mSpecularPower;
	XMFLOAT4 mSpecularColor;
};

#endif