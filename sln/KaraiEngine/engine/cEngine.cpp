#include "cEngine.h"

#include "../global/global.h"
#include "../global/common.h"

#include "../storage/sqlite/cSQLite.h"

#include "../utils/cMaths.h"
cMaths* g_pMaths;

#include "../system/threads/cThreadAnimations.h"
cThreadAnimations* g_pThreadAnimations;

#include "../graphics/models/cModelManager.h"
cModelManager* g_pModelManager = nullptr;

#include "../graphics/shaders/cShaderManager.h"
cShaderManager* g_pShaderManager = nullptr;

#include "../graphics/textures/cTextureManager.h"
cTextureManager* g_pTextureManager = nullptr;

#include "../objects/effects/cParticleManager.h"
cParticleManager* g_pParticleManager = nullptr;

#include "../objects/audio/cAudioInstaller.h"
cAudioInstaller* g_pAudioInstaller = nullptr;

#include "../objects/object_3D/cObject3D.h"

#include "../objects/light/cLightManager.h"
#include "../objects/light/cLightHelper.h"
cLightManager* g_pLightManager = nullptr;

#include "../objects/camera/cCamera.h"
cCamera* g_pCamera = nullptr;

#include "cRenderManager.h"
cRenderManager* g_pRenderManager = nullptr;

#include "cInputManager.h"
cInputManager* g_pInputManager = nullptr;

#include "cLoadManager.h"
cLoadManager* g_pLoadManager = nullptr;

#include "cPoolManager.h"
cPoolManager* g_pPoolManager = nullptr;

#include "../physics/cPhysicsManager.h"
cPhysicsManager* g_pPhysicsManager = nullptr;

#include "../commands/cCommandManager.h"
cCommandManager* g_pCommandManager = nullptr;

#include "../commands/cGroupManager.h"
cGroupManager* g_pGroupManager = nullptr;

#include "../scenes/cSceneManager.h"
cSceneManager* g_pSceneManager = nullptr;

#include "../system/window/cConsoleLogs.h"
cConsoleLogs* g_pConsole = nullptr;

#include "../utils/cQuaternionUtils.h"
cQuaternionUtils* g_pQuatUtils = nullptr;

#include "../utils/cRandomUtils.h"
cRandomUtils* g_pRandUtils = nullptr;

#include "../system/lua/cLuaBrain.h"
cLuaBrain g_LuaBrain;

#include "../system/time/cTime.h"
cTime* g_pTime = nullptr;

#include "../storage/sqlite/cSQLite.h"
#include "../storage/cConfigurator.h"
cConfigurator* g_pConfigurator = nullptr;

#include "../world/terrain/cTerrain.h"
cTerrain* g_pTerrain;
#include "../world/skybox/cSkybox.h"
cSkybox* g_pSkybox;
#include "../world/flora/cFlora.h"
cFlora* g_pFlora;

#define STB_IMAGE_IMPLEMENTATION
#include <other/stb_image.h>

#include <iostream>
#include <sstream>

cObject3D* g_pFindMeshByFriendlyName(std::string friendlyName)
{
    for (unsigned int index = 0; index != g_vpObjectToDraw.size(); index++)
    {
        if (g_vpObjectToDraw[index]->friendlyName == friendlyName)
        {
            return g_vpObjectToDraw[index];
        }
    }
    return NULL;
}

cEngine::cEngine()
{
    this->m_window = nullptr;

    this->m_width = 0;
    this->m_height = 0;
    this->m_currentTime = 0.0f;
}

cEngine::~cEngine(){}

bool cEngine::Initialize(GLFWwindow* window)
{
    this->m_window = window;

    glfwGetWindowSize(m_window, &m_width, &m_height);

    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //glClearColor(0.53f, 0.8f, 0.92f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    g_pConsole = cConsoleLogs::GetConsoleLogs();

    // SQLite example for project and for check functions;
    //this->m_SQLiteExample();

    g_pShaderManager = cShaderManager::GetShaderManager();
    g_pShaderManager->SetBasePath("../../extern/assets/shaders");

    if (!g_pShaderManager->CreateProgramFromFile("MainShader", "mainShader.vert", "mainShader.frag", "mainShader.geom"))
    {
        std::string error = g_pShaderManager->GetLastError();
        g_pConsole->cError("Error: Couldn't compile or link");
        g_pConsole->cError(error);

        return false;
    }
    if (!g_pShaderManager->CreateProgramFromFile("DebugShader", "debugShader.vert", "debugShader.frag"))
    {
        std::string error = g_pShaderManager->GetLastError();
        g_pConsole->cError("Error: Couldn't compile or link");
        g_pConsole->cError(error);

        return false;
    }
    if (!g_pShaderManager->CreateProgramFromFile("TerrainShader", "terrainShader.vert", "terrainShader.frag"))
    {
        std::string error = g_pShaderManager->GetLastError();
        g_pConsole->cError("Error: Couldn't compile or link");
        g_pConsole->cError(error);

        return false;
    }
    if (!g_pShaderManager->CreateProgramFromFile("SkyboxShader", "skyboxShader.vert", "skyboxShader.frag"))
    {
        std::string error = g_pShaderManager->GetLastError();
        g_pConsole->cError("Error: Couldn't compile or link");
        g_pConsole->cError(error);

        return false;
    }
    if (!g_pShaderManager->CreateProgramFromFile("ShadowShader", "shadowShader.vert", "shadowShader.frag"))
    {
        std::string error = g_pShaderManager->GetLastError();
        g_pConsole->cError("Error: Couldn't compile or link");
        g_pConsole->cError(error);

        return false;
    }

    g_pShaderManager->UseShader("MainShader");
    g_pShaderManager->InitActiveUniforms();

///////////////////////////////////////////////////////////////////////////////

    g_pMaths = new cMaths();

    g_pFlora = new cFlora();

    g_pSkybox = new cSkybox();

    g_pTerrain = new cTerrain();

    g_pRandUtils = new cRandomUtils();

    g_pQuatUtils = new cQuaternionUtils();

    g_pGroupManager = new cGroupManager();

    g_pCamera = cCamera::GetCamera(m_window);

    g_pCommandManager = new cCommandManager();

    g_pThreadAnimations = new cThreadAnimations();

    g_pPoolManager = cPoolManager::GetPoolManager();

    g_pLoadManager = cLoadManager::GetLoadManager();

    g_pModelManager = cModelManager::GetModelManager();

    g_pLightManager = cLightManager::GetLightManager();

    g_pSceneManager = cSceneManager::GetSceneManager();

    g_pRenderManager = cRenderManager::GetRenderManager();

    g_pPhysicsManager = cPhysicsManager::GetPhysicsManager();

    g_pAudioInstaller = cAudioInstaller::GetAudioInstaller();

    g_pTextureManager = cTextureManager::GetTextureManager();

    g_pInputManager = cInputManager::GetInputManager(m_window);

    g_pParticleManager = cParticleManager::GetParticleManager();

///////////////////////////////////////////////////////////////////////////////

    m_currentTime = 0.0f;

    g_pTime = new cTime(30);

    g_pConfigurator = new cConfigurator();

    g_pConfigurator->ReadConfigFromFile();

    g_pLoadManager->LoadModels(1);
    g_pLoadManager->InitModels(1);

    g_pAudioInstaller->Init();

    g_pLightManager->Init();

    g_pTerrain->SetBasePath("../../extern/assets/textures/terrain");
    g_pTerrain->SetTerrainSettings(25.0f, 0.0f);
    g_pTerrain->LoadTerrainTextures("grass_diffuse01.jpg", "terrain_height_01_256m.png");

    g_pSkybox->SetBasePath("../../extern/assets/textures/skybox");
    g_pSkybox->LoadSkyboxTextures("MiddaySkybox", false,
                                  "Daylight Box_Right.bmp", "Daylight Box_Left.bmp",
                                  "Daylight Box_Top.bmp", "Daylight Box_Bottom.bmp",
                                  "Daylight Box_Front.bmp", "Daylight Box_Back.bmp");

    g_pSkybox->LoadSkyboxTextures("MidnightSkybox", true,
                                  "SpaceBox_right1_posX.jpg", "SpaceBox_left2_negX.jpg",
                                  "SpaceBox_top3_posY.jpg", "SpaceBox_bottom4_negY.jpg",
                                  "SpaceBox_front5_posZ.jpg", "SpaceBox_back6_negZ.jpg");

    g_pSkybox->SetSkyboxUpdate(3.0f, 0.0f, 10.0f, 5.0f);

    g_pFlora->InitFloraElements();

    g_pSceneManager->Init(1, m_window);

    g_pPhysicsManager->CreateAABBdebSpheres();

    //g_pFBXLoader = new cFBXLoader();
    //g_pFBXLoader->LoadFBXScene("../../extern/assets/animations/paladin/Floating.fbx");

    return true;
}

void cEngine::Update()
{
    m_deltaTime = g_pTime->GetDeltaTime();

    g_pInputManager->KeyboardInput(m_deltaTime);

    g_pPhysicsManager->Update(m_deltaTime);

    g_pParticleManager->Update(m_deltaTime);

    g_pSceneManager->Update(1, m_deltaTime);

    g_pFlora->UpdateFlora(m_deltaTime);

    g_pLightManager->Update();

    g_pCamera->Update();

    g_pAudioInstaller->Update(m_deltaTime);

///////////////////////////////////////////////////////////////////////////////////////

    std::stringstream ssTitle;

    //ssTitle << "Time: " << currentTime;

    if (g_pCamera->GetCameraMod() == DYNAMIC)
    {
        ssTitle << " /// Camera (x,y,z): "
            << -WORLD_POS.x + g_pCamera->GetPosition().x << ", "
            << -WORLD_POS.y + g_pCamera->GetPosition().y << ", "
            << -WORLD_POS.z + g_pCamera->GetPosition().z << " /// "

            << "Dir (x,y,z): "
            << g_pCamera->GetDirection().x << ", "
            << g_pCamera->GetDirection().y << ", "
            << g_pCamera->GetDirection().z << " /// ";
    }
    if (g_pInputManager->CAPS_LOCK_MODE)
    {
        ssTitle << "Model[ " << g_vpObjectToDraw[g_SelectedObject]->friendlyName << " ] "

            << "Pos (x,y,z): "
            << g_vpObjectToDraw[g_SelectedObject]->GetPosition().x << ", "
            << g_vpObjectToDraw[g_SelectedObject]->GetPosition().y << ", "
            << g_vpObjectToDraw[g_SelectedObject]->GetPosition().z << " /// "

            << "Rot (x,y,z): "
            << static_cast<int>(glm::degrees(g_vpObjectToDraw[g_SelectedObject]->GetEulerOrientation().x)) % 360 << ", "
            << static_cast<int>(glm::degrees(g_vpObjectToDraw[g_SelectedObject]->GetEulerOrientation().y)) % 360 << ", "
            << static_cast<int>(glm::degrees(g_vpObjectToDraw[g_SelectedObject]->GetEulerOrientation().z)) % 360 << " /// "

            << "Scale (x,y,z): "
            << g_vpObjectToDraw[g_SelectedObject]->GetScale().x << ", "
            << g_vpObjectToDraw[g_SelectedObject]->GetScale().y << ", "
            << g_vpObjectToDraw[g_SelectedObject]->GetScale().z << " /// ";
    }
    if (g_pInputManager->TAB_MODE)
    {
        ssTitle << "Light[ " << ::g_SelectedLight << " ]: "

            << g_pLightManager->GetLight(g_SelectedLight)->position.x << ", "
            << g_pLightManager->GetLight(g_SelectedLight)->position.y << ", "
            << g_pLightManager->GetLight(g_SelectedLight)->position.z << " / "
            << "const:" << g_pLightManager->GetLight(g_SelectedLight)->atten.x << " / "
            << "linear:" << g_pLightManager->GetLight(g_SelectedLight)->atten.y << " / "
            << "quad:" << g_pLightManager->GetLight(g_SelectedLight)->atten.z << " / "
            << "inner: " << g_pLightManager->GetLight(g_SelectedLight)->param1.y << " / "
            << "outer: " << g_pLightManager->GetLight(g_SelectedLight)->param1.z << " / "

            << "Dir (x,y,z): "
            << g_pLightManager->GetLight(g_SelectedLight)->direction.x << ", "
            << g_pLightManager->GetLight(g_SelectedLight)->direction.y << ", "
            << g_pLightManager->GetLight(g_SelectedLight)->direction.z << " /// ";
           
    }

    std::string theTitle = ssTitle.str();

    m_currentTime += m_deltaTime;

    glfwSetWindowTitle(m_window, theTitle.c_str());
}

void cEngine::Render()
{
    // Render terrain with another shader
    g_pTerrain->RenderTerrain();

    // Render all global objects
    for (unsigned int index = 0; index != g_vpObjectToDraw.size(); index++)
    {
        cObject3D* pCurrentObject = g_vpObjectToDraw[index];

        if (pCurrentObject->bIsVisible)
        {
            glm::mat4 matModel = glm::mat4(1);

            g_pRenderManager->DrawObject(pCurrentObject, matModel);
        }
    }

    // Render all debug objects
    if (g_bDrawDebugObjects)
    {
        for (unsigned int index = 0; index != g_vpDebugObjectToDraw.size(); index++)
        {
            cObject3D* pCurrentObject = g_vpDebugObjectToDraw[index];

            if (pCurrentObject->bIsVisible)
            {
                glm::mat4 matModel = glm::mat4(1);

                g_pRenderManager->DrawObject(pCurrentObject, matModel);
            }
        }
    }

    // Render all local (specific) objects
    for (unsigned int index = 0; index != g_vpLocalObjectToDraw.size(); index++)
    {
        cObject3D* pCurrentObject = g_vpLocalObjectToDraw[index];

        if (pCurrentObject->bIsVisible)
        {
            glm::mat4 matModel = glm::mat4(1);

            g_pRenderManager->DrawObject(pCurrentObject, matModel);
        }
    }

    // Render all particles
    for (unsigned int index = 0; index != g_pParticleManager->m_vecEmitters.size(); index++)
    {
        std::string emitterName = g_pParticleManager->m_vecEmitters[index].friendlyName;
        int numOfParticles = g_pParticleManager->m_vecEmitters[index].maxNumParticles;
        if (g_pParticleManager->m_vecEmitters[index].m_IsVisible)
        {
            for (unsigned int k = 0; k != numOfParticles; k++)
            {
                glm::mat4 matModel = glm::mat4(1);

                g_pRenderManager->DrawObject(g_pParticleManager->GetParticleMesh(emitterName, k), matModel);
            }
        }
    }
    
    // Render light debug spheres
    g_pRenderManager->DrawLightDebugSpheres();    
    
    // Render skybox with another shader
    g_pSkybox->RenderSkybox(m_deltaTime);
}

void cEngine::WindowResize(int width, int height)
{
    g_pCamera->SetWindowSize(width, height);
}

void cEngine::CursorCallback(double xpos, double ypos)
{
    g_pCamera->SetCursorPos(xpos, ypos);
}

void cEngine::ScrollCallback(double xoffset, double yoffset)
{
    g_pInputManager->ScrollCallback(xoffset, yoffset);
}

void cEngine::MouseCallback(int button, int action, int mods)
{
    g_pInputManager->MouseCallback(button, action, mods);
}

void cEngine::KeyCallback(int key, int scancode, int action, int mods)
{
    g_pInputManager->KeyCallback(key, scancode, action, mods);
}

void cEngine::m_SQLiteExample()
{
    cSQLite* m_SQLite = cSQLite::GetSQLite();

    int enemyHealth = -1;
    std::string enemyName = "";

    m_SQLite->GetEnemyData(5, enemyName, enemyHealth);

    m_SQLite->SetEnemyData("Vampire", 1000);
    m_SQLite->SetEnemyData("Vampire", 200);
    m_SQLite->SetEnemyData("Vampire", 50000);

    m_SQLite->GetEnemyData(1, enemyName, enemyHealth);
    g_pConsole->cOutput("Enemy name: " + enemyName + ", health: " + std::to_string(enemyHealth));

    m_SQLite->SetEnemyData("Warlok", 10000);
    m_SQLite->SetEnemyData("Warlok", 8080);
    m_SQLite->GetEnemyData(2, enemyName, enemyHealth);
    g_pConsole->cOutput("Enemy name: " + enemyName + ", health: " + std::to_string(enemyHealth));

    delete m_SQLite;
}

