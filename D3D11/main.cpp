#include <stdlib.h>      
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "System.h"
 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int iCmdShow)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	//_CrtSetBreakAlloc(902);

	System* pSystem = new System;
	Check(pSystem->Init());

	pSystem->Run();
	pSystem->Shutdown();
	SafeDelete(pSystem);

	//At the end of entry point, call _CrtDumpMemoryLeaks func to detect memory leak.
	_CrtDumpMemoryLeaks();

	return 0;
}