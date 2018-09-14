#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>
#pragma comment(lib, "winmm.lib")

class Timer
{
public:
	Timer();
	Timer(const Timer& other);
	~Timer();

	bool Init();
	void Frame();

	float GetFrameTime() const;

private:
	INT64 mFrequency;
	INT64 mStartTime;
	float mFrameTime;
	float mTicksPerMs;

	int mFps;
	int mCpuRate;
};

#endif
