#include "Shader.h"

Shader::Shader()
{
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mLayout = nullptr;

	mMatrixBuffer = nullptr;
	mLightBuffer = nullptr;
	mCameraBuffer = nullptr;

	mSampleState = nullptr;
}

Shader::Shader(const Shader& other)
{
}

Shader::~Shader()
{
}

bool Shader::Init(HWND hwnd, ID3D11Device* device)
{
	Check(InitShader(hwnd, device, L"Shaders/light.vs", L"Shaders/light.ps"));

	return true;
}

bool Shader::Render(ID3D11DeviceContext* context, int indexCount, 
	XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, 
	ID3D11ShaderResourceView* texture, 
	XMFLOAT4 ambientColor, XMFLOAT4 lightColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor,
	XMFLOAT3 cameraPos)
{
	Check(SetShaderParameters(context, world, view, proj, texture, ambientColor, lightColor, lightDir, specularPower, specularColor, cameraPos));

	RenderShader(context, indexCount);

	return true;
}

void Shader::Shutdown()
{
	ShutdownShader();
}

bool Shader::InitShader(HWND hwnd, ID3D11Device* device, WCHAR* vsPath, WCHAR* psPath)
{
	HRESULT result;

	ID3D10Blob* errorMsg = nullptr;
	ID3D10Blob* vsBuffer = nullptr;

	result = D3DCompileFromFile(vsPath, NULL, NULL, "VS", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vsBuffer, &errorMsg);
	
	if (FAILED(result))
	{
		if (errorMsg)
			OutputShaderErrorMessage(hwnd, errorMsg, vsPath);
		else
			MessageBox(hwnd, vsPath, L"Missing Shader File.", MB_OK);

		return false;
	}
		
	ID3D10Blob* psBuffer = nullptr;
	result = D3DCompileFromFile(psPath, NULL, NULL, "PS", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &psBuffer, &errorMsg);
	if (FAILED(result))
	{
		if (errorMsg)
			OutputShaderErrorMessage(hwnd, errorMsg, psPath);
		else
			MessageBox(hwnd, psPath, L"Missing Shader File.", MB_OK);

		return false;
	}

	HR(device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &mVertexShader));
	HR(device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &mPixelShader));

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	HR(device->CreateInputLayout(polygonLayout, numElements, vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), &mLayout));

	ReleaseCOM(vsBuffer);
	ReleaseCOM(psBuffer);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	HR(device->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer));

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	HR(device->CreateBuffer(&lightBufferDesc, NULL, &mLightBuffer));

	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	HR(device->CreateBuffer(&cameraBufferDesc, NULL, &mCameraBuffer));

	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &mSampleState));

	return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* context, XMFLOAT4X4 world,XMFLOAT4X4 view, XMFLOAT4X4 proj, 
	ID3D11ShaderResourceView* texture, 
	XMFLOAT4 ambientColor, XMFLOAT4 lightColor, XMFLOAT3 lightDir, float specularPower, XMFLOAT4 specularColor,
	XMFLOAT3 cameraPos)
{
	XMMATRIX w = XMMatrixTranspose(XMLoadFloat4x4(&world));
	XMMATRIX v = XMMatrixTranspose(XMLoadFloat4x4(&view));
	XMMATRIX p = XMMatrixTranspose(XMLoadFloat4x4(&proj));

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HR(context->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
	MatrixBuffer* mdataPtr = (MatrixBuffer*)mappedRes.pData;
	XMStoreFloat4x4(&(mdataPtr->worldMatrix), w);
	XMStoreFloat4x4(&(mdataPtr->viewMatrix), v);
	XMStoreFloat4x4(&(mdataPtr->projMatrix), p);
	context->Unmap(mMatrixBuffer, 0);
	context->VSSetConstantBuffers(0, 1, &mMatrixBuffer);

	ZeroMemory(&mappedRes, sizeof(D3D11_MAPPED_SUBRESOURCE));	
	HR(context->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
	LightBuffer* ldataPtr = (LightBuffer*)mappedRes.pData;
	ldataPtr->ambientColor = ambientColor;
	ldataPtr->diffuseColor = lightColor;
	ldataPtr->diffuseDir = lightDir;
	ldataPtr->specularPower = specularPower;
	ldataPtr->specularColor = specularColor;
	context->Unmap(mLightBuffer, 0);
	context->PSSetConstantBuffers(0, 1, &mLightBuffer);

	ZeroMemory(&mappedRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	HR(context->Map(mCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));
	CameraBuffer* cdataPtr = (CameraBuffer*)mappedRes.pData;
	cdataPtr->cameraPos = cameraPos;
	cdataPtr->padding = 0.0f;
	context->Unmap(mCameraBuffer, 0);
	context->VSSetConstantBuffers(1, 1, &mCameraBuffer);

	context->PSSetShaderResources(0, 1, &texture);

	return true;
}

void Shader::RenderShader(ID3D11DeviceContext* context, int indexCount)
{
	context->IASetInputLayout(mLayout);

	context->VSSetShader(mVertexShader, NULL, 0);
	context->PSSetShader(mPixelShader, NULL, 0);
	context->PSSetSamplers(0, 1, &mSampleState);

	context->DrawIndexed(indexCount, 0, 0);
}

void Shader::ShutdownShader()
{
	ReleaseCOM(mMatrixBuffer);
	ReleaseCOM(mLightBuffer);
	ReleaseCOM(mCameraBuffer);
	ReleaseCOM(mLayout);
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mVertexShader);
}

void Shader::OutputShaderErrorMessage(HWND hwnd, ID3D10Blob* error, WCHAR* path)
{
	char* errorInfo = static_cast<char*>(error->GetBufferPointer());
	SIZE_T size = error->GetBufferSize();

	std::ofstream fout("shader_compile_log.txt", std::ios::out);
	for (unsigned long i = 0; i < size; ++i)
		fout << errorInfo[i];
	fout.close();

	ReleaseCOM(error);
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", path, MB_OK);
}