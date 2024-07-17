#include "cSceneManager.h"

#include "../global/global.h"

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "cScene01.h"
cScene01 Scene01;

cSceneManager* cSceneManager::m_pSceneManager = nullptr;

cSceneManager* cSceneManager::GetSceneManager()
{
    if (cSceneManager::m_pSceneManager == nullptr)
    {
        cSceneManager::m_pSceneManager = new cSceneManager();
    }
    return cSceneManager::m_pSceneManager;
}

cSceneManager::cSceneManager()
{
    m_Window = nullptr;
}

cSceneManager::~cSceneManager(){}

void cSceneManager::Init(int numScene, GLFWwindow* window)
{
    this->m_Window = window;

    if (numScene == 0)
    {
        m_DefaultSceneInit();
    }
    if (numScene == 1)
    {
        Scene01.Init(m_Window);
    }
}

void cSceneManager::Update(int numScene, float deltaTime)
{
    if (numScene == 0)
    {

    }
    if (numScene == 1)
    {
        Scene01.Update(deltaTime);
    }
}

void cSceneManager::Input(int event)
{
    Scene01.InputKey(event);
}

void cSceneManager::m_DefaultSceneInit()
{

}