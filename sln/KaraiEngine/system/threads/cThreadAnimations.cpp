#include "cThreadAnimations.h"

cThreadAnimations::cThreadAnimations()
{
	this->m_vpThreadAnimInfo.reserve(NUM_OF_THREADS);

	for (size_t index = 0; index < NUM_OF_THREADS; index++)
	{
		sAnimInfo* pAnimInfo = new sAnimInfo();
		pAnimInfo->animator = nullptr;
		pAnimInfo->bIsAlive = true;
		pAnimInfo->bIsRunning = false;
		pAnimInfo->deltaTime = 0.0f;
		pAnimInfo->sleepTime = 0.0;
		m_vpThreadAnimInfo.push_back(pAnimInfo);
	}
}

cThreadAnimations::~cThreadAnimations()
{
	for (size_t i = 0; i < NUM_OF_THREADS; i++)
	{
		CloseHandle(ThreadHandles[i]);
	}
	for (size_t index = 0; index < m_vpThreadAnimInfo.size(); index++)
	{
		delete m_vpThreadAnimInfo[index];
	}
}

DWORD WINAPI UpdateAnimThreads(LPVOID lpParameter)
{
	sAnimInfo* pAnimInfo = static_cast<sAnimInfo*>(lpParameter);

	while (pAnimInfo->bIsAlive)
	{
		if (pAnimInfo->bIsRunning)
		{
			if (pAnimInfo->animator != nullptr)
			{
				pAnimInfo->animator->UpdateAnimation(pAnimInfo->deltaTime);
			}

			pAnimInfo->bIsRunning = false;
		}
		else
		{
			Sleep(pAnimInfo->sleepTime);
		}
	}

	return 0;
}

void cThreadAnimations::InitThreads()
{
	for (size_t index = 0; index < NUM_OF_THREADS; index++)
	{
		void* lpParams = static_cast<void*>(m_vpThreadAnimInfo[index]);

		ThreadHandles[index] = CreateThread(
			NULL,					// lpThreadAttributes,
			0,						// dwStackSize,
			UpdateAnimThreads,		// lpStartAddress,
			lpParams,				// lpParameter,
			0,						// dwCreationFlags,
			&(ThreadIds[index]));	// lpThreadId
	}
}

// Multi thread function
void cThreadAnimations::UpdateThreads(float deltaTime)
{
	size_t vecAnimInfoSize = m_vpThreadAnimInfo.size();
	size_t vecAnimatorSize = m_vpAllAnimators.size();

	unsigned int animatorIndex = 0;

	while (true)
	{
		int finishedThreads = 0;

		for (size_t index = 0; index < vecAnimInfoSize; index++)
		{
			if ( ! m_vpThreadAnimInfo[index]->bIsRunning )
			{
				if (animatorIndex >= vecAnimatorSize)
				{
					finishedThreads++;
				}
				else
				{
					for (size_t k = animatorIndex; k < vecAnimatorSize; k++)
					{
						if (m_vpAllAnimators[k]->GetRunningFlag())
						{
							m_vpThreadAnimInfo[index]->animator = m_vpAllAnimators[k];
							m_vpThreadAnimInfo[index]->deltaTime = deltaTime;
							m_vpThreadAnimInfo[index]->bIsRunning = true;
							animatorIndex++;
							break;
						}

						animatorIndex++;
					}
				}
			}
		}

		if (finishedThreads >= vecAnimInfoSize)
		{
			break;
		}
	}

	return;
}

// One thread function
void cThreadAnimations::UpdateAnimators(float deltaTime)
{
	size_t vecSize = m_vpAllAnimators.size();

	for (size_t index = 0; index < vecSize; index++)
	{
		if (m_vpAllAnimators[index] != nullptr)
		{
			m_vpAllAnimators[index]->UpdateAnimation(deltaTime);
		}
	}

	return;
}

void cThreadAnimations::AddAnimator(cAnimator* animator)
{
	if (animator == nullptr) return;

	m_vpAllAnimators.push_back(animator);
}