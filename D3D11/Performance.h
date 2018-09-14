#ifndef _PERFORMANCE_H_
#define _PERFORMANCE_H_

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib") // Performance data handle

#include <pdh.h>
#include <windows.h>
#include <mmsystem.h>

class Performance
{
public:
	Performance();
	Performance(const Performance& other);
	~Performance();

	void Init();
	void Frame();
	void Shutdown();
	int GetFps() const;
	int GetCpuRate() const;

private:
	unsigned long mStartTime;

	// Calc Fps
	int mFps, mCount;

	// Calc CpuRate
	bool mCanReadCpu;
	HQUERY mQueryHandle;
	HCOUNTER mCounterHandle;
	long mCpuUsage;
};

#endif