#include "Performance.h"

Performance::Performance()
{
}

Performance::Performance(const Performance&)
{
}

Performance::~Performance()
{
}

void Performance::Init()
{
	// Fps
	mCount = 0;
	mFps = 0;

	// CpuRate
	mCanReadCpu = true;
	PDH_STATUS pdhStatus;
	pdhStatus = PdhOpenQuery(NULL, 0, &mQueryHandle);
	if (pdhStatus != ERROR_SUCCESS)
		mCanReadCpu = false;

	pdhStatus = PdhAddCounter(mQueryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &mCounterHandle);
	if (pdhStatus != ERROR_SUCCESS)
		mCanReadCpu = false;
	mCpuUsage = 0;

	mStartTime = timeGetTime();
}

void Performance::Shutdown()
{
	if (mCanReadCpu)
		PdhCloseQuery(mQueryHandle);
}

void Performance::Frame()
{
	++mCount;

	if (timeGetTime() >= (mStartTime + 1000))
	{
		mStartTime = timeGetTime();
		// calc fps
		mFps = mCount;
		mCount = 0;

		// calc cpu rate
		if (mCanReadCpu)
		{
			PdhCollectQueryData(mQueryHandle);
			PDH_FMT_COUNTERVALUE value;
			PdhGetFormattedCounterValue(mCounterHandle, PDH_FMT_LONG, NULL, &value);
			mCpuUsage = value.longValue;
		}
	}
}

int Performance::GetFps() const
{
	return mFps;
}

int Performance::GetCpuRate() const
{
	int usage;
	if (mCanReadCpu)
		usage = static_cast<int>(mCpuUsage);
	else
		usage = -1;

	return usage;
}