#include "System.h"

System::System()
{
	mTimer = nullptr;
	mInput = nullptr;
	mSound = nullptr;
	mGraphics = nullptr;
	mPerformance = nullptr;
	mPositon = nullptr;
}

System::System(const System& other)
{
}

System::~System()
{
}

bool System::Init()
{
	int screenWidth = 0;
	int screenHeight = 0;

	InitWindow(screenWidth, screenHeight);

	mTimer = new Timer;
	Check(mTimer->Init());

	mPerformance = new Performance;
	mPerformance->Init();

	mInput = new Input;
	Check(mInput->Init(mhInstance, mhWnd, screenWidth, screenHeight));

	mGraphics = new Graphics;
	Check(mGraphics->Init(mhWnd, screenWidth, screenHeight));

	mSound = new Sound;
	Check(mSound->Init(mhWnd));

	mPositon = new Position;

	return true;
}

void System::Run()
{
	bool done, result;
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

		if (mInput->IsEscapePressed())
		{
			done = true;
		}
	}


}

void System::Shutdown()
{
	SafeDelete(mPositon);
	SafeDelete(mTimer);

	if (mPerformance)
	{
		mPerformance->Shutdown();
		SafeDelete(mPerformance);
	}

	if (mInput)
	{
		mInput->Shutdown();
		SafeDelete(mInput);
	}

	if (mSound)
	{
		mSound->Shutdown();
		SafeDelete(mSound);
	}

	if (mGraphics)
	{
		mGraphics->Shutdown();
		SafeDelete(mGraphics);
	}

	ShutdownWindow();
}

bool System::Frame()
{
	mTimer->Frame();
	mPerformance->Frame();

	mPositon->SetFrameTime(mTimer->GetFrameTime());
	mPositon->TurnLeft(mInput->IsLeftArrowPressed());
	mPositon->TurnRight(mInput->IsRightArrowPressed());
	float rotY = mPositon->GetRotation();

	Check(mInput->Frame());

	Check(mGraphics->Frame(mTimer->GetFrameTime(), mPerformance->GetFps(), mPerformance->GetCpuRate(), rotY));
	Check(mGraphics->Render());

	return true;
}

void System::InitWindow(/*out*/int& width, /*out*/int& height)
{
	mAppName = L"Drimoon";
	mhInstance = GetModuleHandle(NULL);
	gApp = this;

	WNDCLASSEX wc = { 0 };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mAppName;
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);

	DEVMODE dmScreenSettings;
	int posX, posY;
	if (GRAPHICS_FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(width);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(height);
		dmScreenSettings.dmBitsPerPel = 32; // bpp,r8g8b8a8
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Changes the settings of the default display device to the specified graphics mode.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		width = 800;
		height = 600;
		posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	}

	mhWnd = CreateWindowEx(WS_EX_APPWINDOW, mAppName, mAppName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION,
		posX, posY, width, height, NULL, NULL, mhInstance, NULL);

	ShowWindow(mhWnd, SW_SHOW);
	SetForegroundWindow(mhWnd);
	SetFocus(mhWnd);
}

void System::ShutdownWindow()
{
	// Fix the display settings if leaving full screen mode.
	if (GRAPHICS_FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	DestroyWindow(mhWnd);
	mhWnd = NULL;

	UnregisterClass(mAppName, mhInstance);
	mhInstance = NULL;

	gApp = nullptr;
}

LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	default:
		return gApp->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}