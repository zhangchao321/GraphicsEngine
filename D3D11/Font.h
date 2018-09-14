#ifndef _FONT_H_
#define _FONT_H_

#include <fstream>
#include <DirectXMath.h>
using namespace DirectX;

#include "Texture.h"

class Font
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

public:
	Font();
	Font(const Font& other);
	~Font();

	bool Init(ID3D11Device* device, char* fontFileName, WCHAR* textureName);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture() const;

	void BuildVertexArray(void* vertices, char* text, float drawX, float drawY);

private:
	bool LoadFontData(char* fontFileName);

private:
	FontType* mFont;
	Texture* mTexture;
};

#endif