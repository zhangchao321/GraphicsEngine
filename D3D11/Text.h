#ifndef _TEXT_H_
#define _TEXT_H_

#include "Font.h"
#include "FontShader.h"

class Text
{
private:
	struct Sentence
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

public:
	Text();
	Text(const Text& other);
	~Text();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd, int screenWidth, int screenHeight, char* fontDataFile, WCHAR* fontDDSFile, int textMaxLength, XMFLOAT4X4 baseViewMatrix);
	bool Render(ID3D11DeviceContext* context, char* text, int posX, int posY, XMFLOAT3 color, XMFLOAT4X4 world, XMFLOAT4X4 ortho);
	void Shutdown();
	void SetFps(int fps);
	void SetCpuRate(int cpuRate);
	void ShowPerformance(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 ortho);

private:
	bool InitSentence(ID3D11Device* device, int maxLength);
	bool UpdateSentence(ID3D11DeviceContext* context, char* text, int posX, int posY, XMFLOAT3 color);
	void ReleaseSentence();
	bool RenderSentence(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 ortho);

private:
	Sentence* mSentence;

	Font* mFont;
	FontShader* mFontShader;

	int mCpuRate, mFps;
	int mScreenWidth, mScreenHeight;
	XMFLOAT4X4 mBaseViewMatrix;
};

#endif