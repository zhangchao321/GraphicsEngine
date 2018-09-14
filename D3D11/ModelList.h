#ifndef _MODELLIST_H_
#define _MODELLIST_H_

#include <cstdlib>
#include <ctime>
#include <DirectXMath.h>
using namespace DirectX;

class ModelList
{
private:
	struct ModelInfo
	{
		XMFLOAT4 color;
		float posX, posY, posZ;
	};

public:
	ModelList();
	ModelList(const ModelList& other);
	~ModelList();

	bool Init(int modelCount);
	void Shutdown();

	int GetModelCount() const;
	void GetModelData(int index, float& posX, float& posY, float& posZ, XMFLOAT4& color) const;

private:
	int mModelCount;
	ModelInfo* mModelInfoList;
};

#endif