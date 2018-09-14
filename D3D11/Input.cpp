#include "Input.h"

Input::Input()
{
	mDInput = nullptr;
	mKeyboard = nullptr;
	mMouse = nullptr;
}

Input::Input(const Input& other)
{
}

Input::~Input()
{
}

bool Input::Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight)
{
	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	mMouseX = 0;
	mMouseY = 0;

	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&mDInput, NULL));

	HR(mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, NULL));
	HR(mKeyboard->SetDataFormat(&c_dfDIKeyboard));
	HR(mKeyboard->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND));
	HR(mKeyboard->Acquire());

	HR(mDInput->CreateDevice(GUID_SysMouse, &mMouse, NULL));
	HR(mMouse->SetDataFormat(&c_dfDIMouse));
	HR(mMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	HR(mMouse->Acquire());

	return true;
}

void Input::Shutdown()
{
	mKeyboard->Unacquire();
	ReleaseCOM(mKeyboard);

	mMouse->Unacquire();
	ReleaseCOM(mMouse);

	ReleaseCOM(mDInput);
}

bool Input::Frame()
{
	Check(ReadKeyboard());
	Check(ReadMouse());

	ProcessInput();

	return true;
}

void Input::ProcessInput()
{
	mMouseX += mMouseState.lX;
	mMouseY += mMouseState.lY;

	if (mMouseX < 0)
		mMouseX = 0;
	if (mMouseY < 0)
		mMouseY = 0;

	if (mMouseX > mScreenWidth)
		mMouseX = mScreenWidth;
	if (mMouseY > mScreenHeight)
		mMouseY = mScreenHeight;
}

bool Input::ReadKeyboard()
{
	HRESULT hr;
	hr = mKeyboard->GetDeviceState(sizeof(mKeyboardState), (LPVOID)&mKeyboardState);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			mKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::ReadMouse()
{
	HRESULT hr;
	hr = mMouse->GetDeviceState(sizeof(mMouseState), (LPVOID)&mMouseState);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			mMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Input::IsEscapePressed() const
{
	if (mKeyboardState[DIK_ESCAPE] & 0x80)
		return true;

	return false;
}

bool Input::IsLeftArrowPressed() const
{
	if (mKeyboardState[DIK_LEFTARROW] & 0x80)
		return true;

	return false;
}

bool Input::IsRightArrowPressed() const
{
	if (mKeyboardState[DIK_RIGHTARROW] & 0x80)
		return true;

	return false;
}

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = mMouseX;
	mouseY = mMouseY;
}