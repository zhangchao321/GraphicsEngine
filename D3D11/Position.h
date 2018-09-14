#ifndef _POSITION_H_
#define _POSITION_H_

#include <cmath>

class Position
{
public:
	Position();
	Position(const Position& other);
	~Position();

	void SetFrameTime(float frameTime);
	float GetRotation() const;

	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);

private:
	float mFrameTime;
	float mRotY;
	float mLeftTurnSpeed, mRightTurnSpeed;
};

#endif