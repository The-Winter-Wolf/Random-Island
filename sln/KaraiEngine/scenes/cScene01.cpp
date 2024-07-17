#include "cScene01.h"

#include "../global/global.h"

#include "../world/player/cPlayer.h"
cPlayer* g_pPlayer = nullptr;

#include "../objects/audio/cAudioInstaller.h"
extern cAudioInstaller* g_pAudioInstaller;

#include "../engine/cRenderManager.h"
extern cRenderManager* g_pRenderManager;

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../world/terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include <glm/gtx/vector_angle.hpp>
#include <iostream>

#include "../world/gameplay/cCommands.h"
cCommands* g_pCommands;

#include "../behaviour/cBehaviourType.h"

// The number of threads is configured in the header file, there
#include "../system/threads/cThreadAnimations.h"
extern cThreadAnimations* g_pThreadAnimations;

cScene01::cScene01()
{
    this->m_Window = nullptr;
}

cScene01::~cScene01()
{
}

void cScene01::Init(GLFWwindow* window)
{
    this->m_Window = window;

    this->m_HandLoad();

    this->m_InitPlayer();

    this->m_InitEnemies();

    // Comment here for one thread animations
    g_pThreadAnimations->InitThreads();
}

void cScene01::Update(float deltaTime)
{
    g_pPlayer->UpdatePlayer(deltaTime);
 
    for (int i = 0; i < m_vpEnemies.size(); i++)
    {
        m_vpEnemies[i]->UpdateEnemy(deltaTime);
    }

    g_pCommands->UpdateObjState(m_vpEnemies);

    // Comment here for one thread animations
    g_pThreadAnimations->UpdateThreads(deltaTime);

    // Comment here for multi thread animations
    //g_pThreadAnimations->UpdateAnimators(deltaTime);
}

void cScene01::InputKey(int event)
{
    if (event == 0){}
    if (event == 1){}
    if (event == 2){}
    if (event == 3){}
    if (event == 4){}
    if (event == 5){}
    if (event == 6){}
    if (event == 7){}
    if (event == 8){}
    if (event == 9){}
    if (event == 10){}
    if (event == 11){}
    if (event == 12){}

    return;
}

void cScene01::m_HandLoad()
{
    return;
}

void cScene01::m_InitPlayer()
{
    g_pPlayer = new cPlayer();
    g_pPlayer->InitPlayer();

    g_pCamera->SetPlayer(g_pPlayer);

    return;
}

void cScene01::m_InitEnemies()
{
    for (int i = 0; i < 5; i++)
    {
        iEnemy* warrok = m_pEnemyBuilder->CreateEnemy(WARROK,i);
        iEnemy* vampire = m_pEnemyBuilder->CreateEnemy(VAMPIRE, i);
        iEnemy* slime = m_pEnemyBuilder->CreateEnemy(SLIME, i);

        m_vpEnemies.push_back(warrok);
        m_vpEnemies.push_back(vampire);
        m_vpEnemies.push_back(slime);
    }

    return;
}
