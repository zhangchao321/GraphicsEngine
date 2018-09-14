#include "Position.h"

Position::Position()
{
	mFrameTime = 0.0f;
	mRotY = 0.0f;
	mLeftTurnSpeed = 0.0f;
	mRightTurnSpeed = 0.0f;
}

Position::Position(const Position& other)
{
}

Position::~Position()
{
}

void Position::SetFrameTime(float frameTime)
{
	mFrameTime = frameTime;
}

float Position::GetRotation() const
{
	return mRotY;
}

void Position::TurnLeft(bool keydown)
{
	if (keydown)
	{
		mLeftTurnSpeed += mFrameTime * 0.01f;
		if (mLeftTurnSpeed > (mFrameTime * 0.15f))
			mLeftTurnSpeed = mFrameTime * 0.15f;
	}
	else
	{
		mLeftTurnSpeed -= mFrameTime * 0.005f;
		if (mLeftTurnSpeed < 0.0f)
			mLeftTurnSpeed = 0.0f;
	}

	mRotY -= mLeftTurnSpeed;
	if (mRotY < 0.0f)
		mRotY += 360.0f;
}

void Position::TurnRight(bool keydown)
{
	if (keydown)
	{
		mRightTurnSpeed += mFrameTime * 0.01f;
		if (mRightTurnSpeed > (mFrameTime * 0.15f))
			mRightTurnSpeed = mFrameTime * 0.15f;
	}
	else
	{
		mRightTurnSpeed -= mFrameTime * 0.005f;
		if (mRightTurnSpeed < 0.0f)
			mRightTurnSpeed = 0.0f;
	}

	mRotY += mRightTurnSpeed;
	if (mRotY > 360.0f)
		mRotY -= 360.0f;
}