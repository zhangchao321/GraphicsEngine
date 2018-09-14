#include "Model.h"

Model::Model()
{
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;

	mTexture = nullptr;
	mModelData = nullptr;
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

bool Model::Init(ID3D11Device* device, char* modelName, WCHAR* textureName)
{
	Check(LoadModel(modelName));

	Check(InitBuffers(device));

	mTexture = new Texture;
	Check(mTexture->Init(device, textureName));

	return true;
}

void Model::Render(ID3D11DeviceContext* context)
{
	RenderBuffers(context);
}

void Model::Shutdown()
{
	if (mTexture)
	{
		mTexture->Shutdown();
		SafeDelete(mTexture);
	}

	if (mModelData)
	{
		delete[] mModelData;
		mModelData = nullptr;
	}

	ShutdownBuffers();
}

bool Model::InitBuffers(ID3D11Device* device)
{
	Vertex* vertices = new Vertex[mVertexCount];
	unsigned long* indices = new unsigned long[mIndexCount];

	for (int i = 0; i < mVertexCount; ++i)
	{
		vertices[i].pos = XMFLOAT3(mModelData[i].x, mModelData[i].y, mModelData[i].z);
		vertices[i].tex = XMFLOAT2(mModelData[i].u, mModelData[i].v);
		vertices[i].normal = XMFLOAT3(mModelData[i].nx, mModelData[i].ny, mModelData[i].nz);

		indices[i] = i;
	}

	D3D11_BUFFER_DESC vbDesc;
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = vertices;
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&vbDesc, &vbData, &mVertexBuffer));

	D3D11_BUFFER_DESC ibDesc;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = indices;
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	HR(device->CreateBuffer(&ibDesc, &ibData, &mIndexBuffer));

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::RenderBuffers(ID3D11DeviceContext* context)
{
	unsigned int stride = sizeof(Vertex);
	unsigned int offset = 0;

	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::ShutdownBuffers()
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

ID3D11ShaderResourceView* Model::GetTexture() const
{
	return mTexture->GetTexture();
}

bool Model::LoadModel(char* modelName)
{
	std::ifstream fin(modelName, std::ios::in);
	if (fin.fail())
		return false;

	char igonre;
	fin.get(igonre);
	while (igonre != ':')
		fin.get(igonre);

	fin >> mVertexCount;
	mIndexCount = mVertexCount;

	mModelData = new ModelType[mVertexCount];
	fin.get(igonre);
	while (igonre != ':')
		fin.get(igonre);

	for (int i = 0; i < mVertexCount; ++i)
	{
		fin >> mModelData[i].x >> mModelData[i].y >> mModelData[i].z;
		fin >> mModelData[i].u >> mModelData[i].v;
		fin >> mModelData[i].nx >> mModelData[i].ny >> mModelData[i].nz;
	}

	fin.close();

	return true;
}