#pragma once

#include "../../global/common.h"

class cTime
{
public:
	cTime(unsigned int numberOfSamples);

	void SetMinFPS(float minFPS);

	float GetDeltaTime();

	float GetGLFWTime();

	float GetFPS();

	std::string GetDateAndTime();


private:
	float m_LastTime;

	float m_MaxDeltaTime;

	std::vector<float> m_vecDeltaTimes;

	unsigned int m_NextBufferIndex = 0;

	static const float MAX_DELTA_TIME_DEFAULT;
};

