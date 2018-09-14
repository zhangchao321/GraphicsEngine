#ifndef _SHADER_H_
#define _SHADER_H_

#include <d3d11.h>
#include <fstream>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Utils.h"

#pragma comment(lib, "D3Dcompiler.lib")

class Shader
{
private:
	struct MatrixBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 viewMatrix;
		XMFLOAT4X4 projMatrix;
	};

	struct LightBuffer
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 diffuseDir;
		float    specularPower;
		XMFLOAT4 specularColor;
	};

	struct CameraBuffer
	{
		XMFLOAT3 cameraPos;
		float padding;
	};

public:
	Shader();
	Shader(const Shader& other);
	~Shader();

	bool Init(HWND hwnd, ID3D11Device* device);
	bool Render(ID3D11DeviceContext* context, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 lightColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor, XMFLOAT3 cameraPos);
	void Shutdown();

private:
	bool InitShader(HWND hwnd, ID3D11Device* device, WCHAR* vsPath, WCHAR* psPath);
	void RenderShader(ID3D11DeviceContext* context, int indexCount);
	void ShutdownShader();

	void OutputShaderErrorMessage(HWND hwnd, ID3D10Blob* blob, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView* texture, XMFLOAT4 ambientColor, XMFLOAT4 lightColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor, XMFLOAT3 cameraPos);

private:
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mLayout;
	ID3D11Buffer* mMatrixBuffer;
	ID3D11Buffer* mLightBuffer;
	ID3D11Buffer* mCameraBuffer;
	ID3D11SamplerState* mSampleState;
};

#endif
