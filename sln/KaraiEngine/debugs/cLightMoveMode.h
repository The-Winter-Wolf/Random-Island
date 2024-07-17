#pragma once
#include "../global/global.h"

#if LIGHT_CHANGE_MODE

enum LightEvents
{
	LIGHT_SELECT,
	LIGHT_BOOST,

	LIGHT_FORWARD,
	LIGHT_BACKWARD,
	LIGHT_RIGHT,
	LIGHT_LEFT,
	LIGHT_UP,
	LIGHT_DOWN,

	LIGHT_ROTATE_FX,
	LIGHT_ROTATE_BX,
	LIGHT_ROTATE_FY,
	LIGHT_ROTATE_BY,
	LIGHT_ROTATE_FZ,
	LIGHT_ROTATE_BZ,

	LIGHT_LINEAR_MORE,
	LIGHT_LINEAR_LESS,

	LIGHT_QUAD_MORE,
	LIGHT_QUAD_LESS,

	LIGHT_OUT_ANGLE_MORE,
	LIGHT_OUT_ANGLE_LESS,

	LIGHT_IN_ANGLE_MORE,
	LIGHT_IN_ANGLE_LESS,
};

class cLightMoveMode
{
public:
	cLightMoveMode();
	~cLightMoveMode();

	void MoveLight(LightEvents event, float eventValue);

private:
	float attenuation;
	float angle;

	float moveSpeed;
	float firstSpeed;
	float secondSpeed;

	int numberOfLights;
};

#endif // LIGHT_CHANGE_MODE

