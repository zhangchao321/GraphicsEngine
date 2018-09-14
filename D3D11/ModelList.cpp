#include "ModelList.h"

ModelList::ModelList()
{
	mModelInfoList = nullptr;
}

ModelList::ModelList(const ModelList& other)
{
}

ModelList::~ModelList()
{
}

int ModelList::GetModelCount() const
{
	return mModelCount;
}

void ModelList::GetModelData(int index, float& posX, float& posY, float& posZ, XMFLOAT4& color) const
{
	posX = mModelInfoList[index].posX;
	posY = mModelInfoList[index].posY;
	posZ = mModelInfoList[index].posZ;

	color = mModelInfoList[index].color;
}

bool ModelList::Init(int modelCount)
{
	mModelCount = modelCount;
	mModelInfoList = new ModelInfo[mModelCount];

	srand((unsigned int)time(NULL));
	float r, g, b;
	for (int i = 0; i < modelCount; ++i)
	{
		r = static_cast<float>(rand() / RAND_MAX);
		g = static_cast<float>(rand() / RAND_MAX);
		b = static_cast<float>(rand() / RAND_MAX);
		mModelInfoList[i].color = XMFLOAT4(r, g, b, 1.0f);

		mModelInfoList[i].posX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		mModelInfoList[i].posY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		mModelInfoList[i].posZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

void ModelList::Shutdown()
{
	delete[] mModelInfoList;
	mModelInfoList = nullptr;
}