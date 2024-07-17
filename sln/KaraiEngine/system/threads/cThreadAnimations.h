#pragma once
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN	

#include "../../animations/cAnimator.h"

struct sAnimInfo
{
	cAnimator* animator = nullptr;
	bool bIsRunning = false;
	bool bIsAlive = true;
	float deltaTime = 0.0f;
	DWORD sleepTime = 0.0;
};

class cThreadAnimations
{
public:
	cThreadAnimations();
	~cThreadAnimations();
	
	void InitThreads();
	void UpdateThreads(float deltaTime);

	void UpdateAnimators(float deltaTime);

	void AddAnimator(cAnimator* animator);

private:
	// The number of threads is set here
	static const unsigned int NUM_OF_THREADS = 11;
	DWORD ThreadIds[NUM_OF_THREADS] = { 0 };
	HANDLE ThreadHandles[NUM_OF_THREADS] = { 0 };

	std::vector<sAnimInfo*> m_vpThreadAnimInfo;
	std::vector<cAnimator*> m_vpAllAnimators;
};

