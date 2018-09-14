#ifndef _MODEL_H_
#define _MODEL_H_

#include <d3d11.h>
#include <fstream>
#include <DirectXMath.h>
using namespace DirectX;

#include "Utils.h"
#include "Texture.h"

class Model
{
private:
	struct Vertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

	// our own 3D Model format file( described in Model/Cube.txt )
	struct ModelType
	{
		float x, y, z;
		float u, v;
		float nx, ny, nz;
	};

public:
	Model();
	Model(const Model& other);
	~Model();

	bool Init(ID3D11Device* device, char* modelName, WCHAR* textureName);
	void Render(ID3D11DeviceContext* context);
	void Shutdown();

	int GetVertexCount() const { return mVertexCount; }
	int GetIndexCount() const { return mIndexCount; }
	ID3D11ShaderResourceView* GetTexture() const;

private:
	bool InitBuffers(ID3D11Device* device);
	void RenderBuffers(ID3D11DeviceContext* context);
	void ShutdownBuffers();

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	int mVertexCount;
	int mIndexCount;

	Texture* mTexture;
	ModelType* mModelData;
};

#endif
