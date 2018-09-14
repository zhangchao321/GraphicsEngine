#include "Font.h"

Font::Font()
{
	mFont = nullptr;
	mTexture = nullptr;
}

Font::Font(const Font& other)
{
}

Font::~Font()
{
}

bool Font::Init(ID3D11Device* device, char* fontFileName, WCHAR* textureName)
{
	Check(LoadFontData(fontFileName));

	// Load Texture
	mTexture = new Texture;
	Check(mTexture->Init(device, textureName));

	return true;
}

void Font::Shutdown()
{
	if (mFont)
	{
		delete[] mFont;
		mFont = nullptr;
	}

	if (mTexture)
	{
		mTexture->Shutdown();		
		SafeDelete(mTexture);
	}
}

ID3D11ShaderResourceView* Font::GetTexture() const
{
	return mTexture->GetTexture();
}

bool Font::LoadFontData(char* fontFileName)
{
	mFont = new FontType[95];
	std::ifstream fin;
	fin.open(fontFileName);
	if (fin.fail())
		return false;

	char input;
	for (int i = 0; i < 95; ++i)
	{
		fin.get(input);
		while (input != ' ')
			fin.get(input);
		fin.get(input);
		while (input != ' ')
			fin.get(input);

		fin >> mFont[i].left;
		fin >> mFont[i].right;
		fin >> mFont[i].size;
	}

	fin.close();

	return true;
}

void Font::BuildVertexArray(void* vertices, char* text, float drawX, float drawY)
{
	Vertex* vertexPtr = (Vertex*)vertices;
	int numLetters = static_cast<int>(strlen(text));

	int letter;
	int index = 0;
	for (int i = 0; i < numLetters; ++i)
	{
		letter = static_cast<int>(text[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if (letter == 0)
			drawX += 3.0f;
		else
		{
			// First triangle in quad
			vertexPtr[index].pos = XMFLOAT3(drawX, drawY, 0.0f); // top-left
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].left, 0.0f);
			index++;

			vertexPtr[index].pos = XMFLOAT3(drawX + mFont[letter].size, drawY - 16, 0.0f); // bottom-right
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].right, 1.0f);
			index++;

			vertexPtr[index].pos = XMFLOAT3(drawX, drawY - 16, 0.0f); // bottom-left
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].left, 1.0f);
			index++;

			// Second triangle in quad
			vertexPtr[index].pos = XMFLOAT3(drawX, drawY, 0.0f); // top-left
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].left, 0.0f);
			index++;

			vertexPtr[index].pos = XMFLOAT3(drawX + mFont[letter].size, drawY, 0.0f); // top-right
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].right, 0.0f);
			index++;

			vertexPtr[index].pos = XMFLOAT3(drawX + mFont[letter].size, drawY - 16, 0.0f); // bottom-right
			vertexPtr[index].tex = XMFLOAT2(mFont[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX += (mFont[letter].size + 1.0f);
		}
	}
}