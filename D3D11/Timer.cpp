#include "Timer.h"

Timer::Timer()
{
}

Timer::Timer(const Timer& other)
{
}

Timer::~Timer()
{
}

bool Timer::Init()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency);
	if (mFrequency == 0)
		return false;

	mTicksPerMs = static_cast<float>(mFrequency) * 0.001f;

	QueryPerformanceCounter((LARGE_INTEGER*)&mStartTime);

	return true;
}


void Timer::Frame()
{
	INT64 currTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	timeDifference = static_cast<float>(currTime - mStartTime);

	mFrameTime = timeDifference / mTicksPerMs;

	mStartTime = currTime;
}

float Timer::GetFrameTime() const
{
	return mFrameTime;
}