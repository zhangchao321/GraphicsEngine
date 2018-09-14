#include "Text.h"

Text::Text()
{
	mSentence = nullptr;

	mFont = nullptr;
	mFontShader = nullptr;
}

Text::Text(const Text& other)
{
}

Text::~Text()
{
}

bool Text::Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd, int screenWidth, int screenHeight, char* fontDataFile, WCHAR* fontDDSFile, int textMaxLength, XMFLOAT4X4 baseViewMatrix)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;
	mBaseViewMatrix = baseViewMatrix;

	mFont = new Font;
	Check(mFont->Init(device, fontDataFile, fontDDSFile));

	mFontShader = new FontShader;
	Check(mFontShader->Init(hwnd, device));

	Check(InitSentence(device, textMaxLength));

	return true;
}

bool Text::Render(ID3D11DeviceContext* context, char* text, int posX, int posY, XMFLOAT3 color, XMFLOAT4X4 world, XMFLOAT4X4 ortho)
{
	Check(UpdateSentence(context, text, posX, posY, color));
	Check(RenderSentence(context, world, ortho));

	return true;
}

void Text::Shutdown()
{
	ReleaseSentence();

	if (mFontShader)
	{
		mFontShader->Shutdown();
		SafeDelete(mFontShader);
	}

	if (mFont)
	{
		mFont->Shutdown();
		SafeDelete(mFont);
	}
}

bool Text::InitSentence(ID3D11Device* device, int maxLength)
{
	mSentence = new Sentence;
	mSentence->vertexBuffer = nullptr;
	mSentence->indexBuffer = nullptr;
	mSentence->maxLength = maxLength;
	mSentence->vertexCount = 6 * maxLength;
	mSentence->indexCount = mSentence->vertexCount;

	Vertex* vertices = new Vertex[mSentence->vertexCount];
	unsigned long* indices = new unsigned long[mSentence->indexCount];

	memset(vertices, 0, sizeof(Vertex) * mSentence->vertexCount);
	for (int i = 0; i < mSentence->indexCount; ++i)
		indices[i] = i;

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(Vertex) * mSentence->vertexCount;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&vbDesc, &vData, &mSentence->vertexBuffer));

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * mSentence->indexCount;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&ibDesc, &iData, &mSentence->indexBuffer));

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

bool Text::UpdateSentence(ID3D11DeviceContext* context, char* text, int posX, int posY, XMFLOAT3 color)
{
	mSentence->red = color.x;
	mSentence->green = color.y;
	mSentence->blue = color.z;

	int numLetters = static_cast<int>(strlen(text));
	if (numLetters > mSentence->maxLength)
		return false;

	Vertex* vertices = new Vertex[mSentence->vertexCount];
	memset(vertices, 0, sizeof(Vertex) * mSentence->vertexCount);

	float drawX = static_cast<float>(posX - (mScreenWidth>>1));
	float drawY = static_cast<float>((mScreenHeight>>1) - posY);

	mFont->BuildVertexArray((void*)vertices, text, drawX, drawY);

	D3D11_MAPPED_SUBRESOURCE mappedRes;
	HR(context->Map(mSentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes));

	Vertex* verticesPtr = (Vertex*)mappedRes.pData;
	memcpy(verticesPtr, (void*)vertices, sizeof(Vertex) * mSentence->vertexCount);
	
	context->Unmap(mSentence->vertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;

	return true;
}

bool Text::RenderSentence(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 ortho)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mSentence->vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mSentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMFLOAT4 pixelColor = XMFLOAT4(mSentence->red, mSentence->green, mSentence->blue, 1.0f);
	Check(mFontShader->Render(context, mSentence->indexCount, world, mBaseViewMatrix, ortho, pixelColor, mFont->GetTexture()));

	return true;
}

void Text::ReleaseSentence()
{
	if (mSentence)
	{
		ReleaseCOM(mSentence->vertexBuffer);
		ReleaseCOM(mSentence->indexBuffer);
		SafeDelete(mSentence);
	}
}

void Text::SetCpuRate(int cpuRate)
{
	mCpuRate = cpuRate;
}

void Text::SetFps(int fps)
{
	mFps = fps;
}

void Text::ShowPerformance(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 ortho)
{
	char fpsStr[16] = "FPS:";
	char cpuStr[16] = "CPU:";
	char tempStr[8];

	// In _itoa_s, parameter '10' means we use Decimal System
	_itoa_s(mFps, tempStr, 10);
	strcat_s(fpsStr, tempStr);

	_itoa_s(mCpuRate, tempStr, 10);
	strcat_s(cpuStr, tempStr);
	strcpy_s(tempStr, "%");
	strcat_s(cpuStr, tempStr);

	Render(context, fpsStr, 20, 20, XMFLOAT3(1.0f, 0.0f, 0.0f), world, ortho);
	Render(context, cpuStr, 20, 40, XMFLOAT3(1.0f, 0.0f, 0.0f), world, ortho);
}