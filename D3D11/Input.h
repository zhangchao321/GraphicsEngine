#ifndef _INPUT_H_
#define _INPUT_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")

#include <dinput.h>

#include "Utils.h"

class Input
{
public:
	Input();
	Input(const Input& other);
	~Input();

	bool Init(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight);
	bool Frame();
	void Shutdown();
	void GetMouseLocation(int& mouseX, int& mouseY);

	bool IsEscapePressed() const;
	bool IsLeftArrowPressed() const;
	bool IsRightArrowPressed() const;

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* mDInput;
	IDirectInputDevice8* mKeyboard;
	IDirectInputDevice8* mMouse;

	unsigned char mKeyboardState[256];
	DIMOUSESTATE mMouseState;

	int mScreenWidth, mScreenHeight;
	int mMouseX, mMouseY;
};
#endif