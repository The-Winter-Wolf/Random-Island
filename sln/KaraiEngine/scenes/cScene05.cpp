#include "cScene05.h"

#include "../global/global.h"

#include "../objects/object_3D/cObject3D.h"
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

#include "../world/player/cPlayer.h"
cPlayer* g_pPlayer = nullptr;

#include "../engine/cRenderManager.h"
extern cRenderManager* g_pRenderManager;

#include "../graphics/textures/cTextureManager.h"
extern cTextureManager* g_pTextureManager;

#include "../graphics/shaders/cShaderManager.h"
extern cShaderManager* g_pShaderManager;

#include "../objects/audio/cAudioInstaller.h"
extern cAudioInstaller* g_pAudioInstaller;

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../physics/cPhysicsManager.h";
extern cPhysicsManager* g_pPhysicsManager;

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../utils/cRandomUtils.h"
extern cRandomUtils* g_pRandUtils;

#include "../world/terrain/cTerrain.h"
extern cTerrain* g_pTerrain;

#include "../system/lua/cLuaBrain.h"
extern cLuaBrain g_LuaBrain;

#include <iostream>

// The number of threads is configured in the header file, there
#include "../system/threads/cThreadAnimations.h"
extern cThreadAnimations* g_pThreadAnimations;

#include "../graphics/FBO/cFBO.h"
cFBO* g_pFBO_1;
cFBO* g_pFBO_2;

#include "../behaviour/cBoids.h"
cBoids* pBoids;

cScene05::cScene05()
{
    this->m_Window = nullptr;
    this->pScreen1 = nullptr;
    this->pScreen2 = nullptr;

    this->m_bScreen01TurnOn = false;
    this->m_bScreen02TurnOn = false;

    this->m_bIsAberration01 = false;
    this->m_bIsNightVision01 = false;
    this->m_bIsBlurScreenBasic01 = false;
    this->m_bIsBlurScreenSlower01 = false;
    this->m_bIsBlurScreenFaster01 = false;

    this->m_bIsAberration02 = false;
    this->m_bIsNightVision02 = false;
    this->m_bIsBlurScreenBasic02 = false;
    this->m_bIsBlurScreenSlower02 = false;
    this->m_bIsBlurScreenFaster02 = false;

    this->warrok01 = nullptr;
    this->warrok02 = nullptr;
    this->warrok03 = nullptr;

    this->vampire01 = nullptr;
    this->vampire02 = nullptr;
    this->vampire03 = nullptr;
}

cScene05::~cScene05() 
{
   delete g_pFBO_1;
   delete g_pFBO_2;
}

void cScene05::Init(GLFWwindow* window)
{
    this->m_Window = window;

    this->m_HandLoad();

    this->m_InitFBO();

    this->m_InitPlayer();

    this->m_InitEnemies();

    pBoids = new cBoids();
    pBoids->InitBoids();

    // Comment here for one thread animations
    g_pThreadAnimations->InitThreads();
}

void cScene05::Update(float deltaTime)
{
    //this->m_UpdateScreen01();
    //this->m_UpdateScreen02();

    this->m_UpdateCamera();

    g_pPlayer->UpdatePlayer(deltaTime);

    pBoids->UpdateBoids(deltaTime);

    // Comment here for one thread animations
    g_pThreadAnimations->UpdateThreads(deltaTime); 

    // Comment here for multi thread animations
    //g_pThreadAnimations->UpdateAnimators(deltaTime);

    //this->m_DisplayScreen01();

    //this->m_DisplayScreen02();
    g_LuaBrain.RunScript("CheckLocalEvent()");

    //if (m_bScreen01TurnOn || m_bScreen02TurnOn)
    //{
    //    g_pAudioInstaller->SetPaused(0, false);
    //}
    //else
    //{
    //    g_pAudioInstaller->SetPaused(0, true);
    //}
}

void cScene05::InputKey(int event)
{
    if (event == 1)
    {
        //if (m_bScreen01TurnOn == true)
        //{
        //    this->m_bScreen01TurnOn = false;
        //}
        //else if (m_bScreen01TurnOn == false)
        //{
        //    this->m_bScreen01TurnOn = true;
        //}
        //return;
    }
    if (event == 2)
    {
        if (m_bIsNightVision01 == true)
        {
            this->m_bIsNightVision01 = false;
        }
        else if (m_bIsNightVision01 == false)
        {
            this->m_bIsNightVision01 = true;
        }
        return;
    }
    if (event == 3)
    {
        if (m_bIsAberration01 == true)
        {
            this->m_bIsAberration01 = false;
        }
        else if (m_bIsAberration01 == false)
        {
            this->m_bIsAberration01 = true;
        }
        return;
    }
    if (event == 4)
    {
        if (m_bIsBlurScreenBasic01 == true)
        {
            this->m_bIsBlurScreenBasic01 = false;
        }
        else if (m_bIsBlurScreenBasic01 == false)
        {
            this->m_bIsBlurScreenBasic01 = true;
        }
        return;
    }
    if (event == 5)
    {
        if (m_bIsBlurScreenSlower01 == true)
        {
            this->m_bIsBlurScreenSlower01 = false;
        }
        else if (m_bIsBlurScreenSlower01 == false)
        {
            this->m_bIsBlurScreenSlower01 = true;
        }
        return;
    }
    if (event == 6)
    {
        if (m_bIsBlurScreenFaster01 == true)
        {
            this->m_bIsBlurScreenFaster01 = false;
        }
        else if (m_bIsBlurScreenFaster01 == false)
        {
            this->m_bIsBlurScreenFaster01 = true;
        }
        return;
    }

    if (event == 7)
    {
        //if (m_bScreen02TurnOn == true)
        //{
        //    this->m_bScreen02TurnOn = false;
        //}
        //else if (m_bScreen02TurnOn == false)
        //{
        //    this->m_bScreen02TurnOn = true;
        //}
        //return;
    }
    if (event == 8)
    {
        if (m_bIsNightVision02 == true)
        {
            this->m_bIsNightVision02 = false;
        }
        else if (m_bIsNightVision02 == false)
        {
            this->m_bIsNightVision02 = true;
        }
        return;
    }
    if (event == 9)
    {
        if (m_bIsAberration02 == true)
        {
            this->m_bIsAberration02 = false;
        }
        else if (m_bIsAberration02 == false)
        {
            this->m_bIsAberration02 = true;
        }
        return;
    }
    if (event == 10)
    {
        if (m_bIsBlurScreenBasic02 == true)
        {
            this->m_bIsBlurScreenBasic02 = false;
        }
        else if (m_bIsBlurScreenBasic02 == false)
        {
            this->m_bIsBlurScreenBasic02 = true;
        }
        return;
    }
    if (event == 11)
    {
        if (m_bIsBlurScreenSlower02 == true)
        {
            this->m_bIsBlurScreenSlower02 = false;
        }
        else if (m_bIsBlurScreenSlower02 == false)
        {
            this->m_bIsBlurScreenSlower02 = true;
        }
        return;
    }
    if (event == 12)
    {
        if (m_bIsBlurScreenFaster02 == true)
        {
            this->m_bIsBlurScreenFaster02 = false;
        }
        else if (m_bIsBlurScreenFaster02 == false)
        {
            this->m_bIsBlurScreenFaster02 = true;
        }
        return;
    }

    if (event == 0)
    {
        m_bScreen01TurnOn = true;
        m_bScreen02TurnOn = true;
    }

    return;
}

void cScene05::m_HandLoad()
{
    return;
}

void cScene05::m_InitFBO()
{
    std::string FBOError;

    g_pFBO_1 = new cFBO();
    if (!g_pFBO_1->InitFBO(1970, 1080, true, true, FBOError))
    {
        g_pConsole->cError("Error creating FBO_1: " + FBOError);
        return;
    }
    else
    {
        g_pConsole->cSuccess("FBO_1 created.");
    }

    g_pFBO_2 = new cFBO();
    if (!g_pFBO_2->InitFBO(1970, 1080, true, true, FBOError))
    {
        g_pConsole->cError("Error creating FBO_2: " + FBOError);
        return;
    }
    else
    {
        g_pConsole->cSuccess("FBO_2 created.");
    }

    pScreen1 = g_pFindMeshByFriendlyName("Screen01");
    pScreen2 = g_pFindMeshByFriendlyName("Screen02");

    return;
}

void cScene05::m_InitPlayer()
{
    cObject3D* player = g_pFindMeshByFriendlyName("Paladin");
    player->GetPhysBody()->SetRigidBody(cPhysicsBody::DYNAMIC);
    player->GetPhysBody()->SetSphereCollider(1.0f);
    player->GetPhysBody()->SetRestitution(0.0f);
    player->GetPhysBody()->SetMass(0.1f);
    g_pPhysicsManager->AddPhysObject(player);

    g_pPlayer = new cPlayer();
    g_pPlayer->SetPlayerObject(player);
    g_pPlayer->InitPlayer();

    g_pCamera->SetPlayer(g_pPlayer);

    return;
}

void cScene05::m_InitEnemies()
{
    this->warrok01 = g_pFindMeshByFriendlyName("Warrok01");
    this->warrok02 = g_pFindMeshByFriendlyName("Warrok02");
    this->warrok03 = g_pFindMeshByFriendlyName("Warrok03");

    this->vampire01 = g_pFindMeshByFriendlyName("Vampire01");
    this->vampire02 = g_pFindMeshByFriendlyName("Vampire02");
    this->vampire03 = g_pFindMeshByFriendlyName("Vampire03");

    cAnimation* pAnimation1 = new cAnimation();
    pAnimation1->SetBasePath("../../extern/assets/animations/warrok");
    pAnimation1->LoadAnimationFromFile("Wave_Hip_Hop_Dance_01.dae", warrok01->GetModel());

    cAnimation* pAnimation2 = new cAnimation();
    pAnimation2->SetBasePath("../../extern/assets/animations/warrok");
    pAnimation2->LoadAnimationFromFile("Wave_Hip_Hop_Dance_02.dae", warrok02->GetModel());

    cAnimation* pAnimation3 = new cAnimation();
    pAnimation3->SetBasePath("../../extern/assets/animations/warrok");
    pAnimation3->LoadAnimationFromFile("Tut_Hip_Hop_Dance.dae", warrok03->GetModel());

    cAnimation* pAnimation4 = new cAnimation();
    pAnimation4->SetBasePath("../../extern/assets/animations/vampire");
    pAnimation4->LoadAnimationFromFile("Belly Dance.dae", vampire01->GetModel());

    cAnimation* pAnimation5 = new cAnimation();
    pAnimation5->SetBasePath("../../extern/assets/animations/vampire");
    pAnimation5->LoadAnimationFromFile("Salsa Dancing.dae", vampire02->GetModel());

    cAnimation* pAnimation6 = new cAnimation();
    pAnimation6->SetBasePath("../../extern/assets/animations/vampire");
    pAnimation6->LoadAnimationFromFile("Swing Dancing.dae", vampire03->GetModel());

    cAnimator* pAnimator1 = new cAnimator(pAnimation1);
    cAnimator* pAnimator2 = new cAnimator(pAnimation2);
    cAnimator* pAnimator3 = new cAnimator(pAnimation3);
    cAnimator* pAnimator4 = new cAnimator(pAnimation4);
    cAnimator* pAnimator5 = new cAnimator(pAnimation5);
    cAnimator* pAnimator6 = new cAnimator(pAnimation6);

    pAnimator1->IsRunning(true);
    pAnimator2->IsRunning(true);
    pAnimator3->IsRunning(true);
    pAnimator4->IsRunning(true);
    pAnimator5->IsRunning(true);
    pAnimator6->IsRunning(true);

    this->warrok01->pAnimator = pAnimator1;
    this->warrok02->pAnimator = pAnimator2;
    this->warrok03->pAnimator = pAnimator3;

    this->vampire01->pAnimator = pAnimator4;
    this->vampire02->pAnimator = pAnimator5;
    this->vampire03->pAnimator = pAnimator6;

    g_pThreadAnimations->AddAnimator(pAnimator1);
    g_pThreadAnimations->AddAnimator(pAnimator2);
    g_pThreadAnimations->AddAnimator(pAnimator3);
    g_pThreadAnimations->AddAnimator(pAnimator4);
    g_pThreadAnimations->AddAnimator(pAnimator5);
    g_pThreadAnimations->AddAnimator(pAnimator6);

    return;
}

void cScene05::m_UpdateCamera()
{
    int width = 0, height = 0;

    glfwGetFramebufferSize(m_Window, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);

    g_pCamera->SetCameraMod(3);

    g_pCamera->SetBufferSize(width, height);

    g_pCamera->Update(g_ShaderID);

    return;
}

void cScene05::m_UpdateScreen01()
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_pFBO_1->ID);
    glViewport(0, 0, g_pFBO_1->width, g_pFBO_1->height);
    g_pFBO_1->ClearBuffers();

    glm::vec3 cameraPos = glm::vec3(523.078f, 100.89f, 2408.58f);
    glm::vec3 lookAt = glm::vec3(523.724f, 98.72f, 2417.13f);
    glm::vec3 direction = lookAt - cameraPos;

    g_pCamera->SetCameraMod(0);
    g_pCamera->SetBufferSize(g_pFBO_1->width, g_pFBO_1->height);
    g_pCamera->SetDirection(direction);
    g_pCamera->SetPosition(cameraPos);
    g_pCamera->Update(g_ShaderID);

    cObject3D* box = g_pFindMeshByFriendlyName("SkyBox");
    box->SetPosition(cameraPos);

    glm::mat4 matModel = glm::mat4(1.0);
    g_pRenderManager->DrawObject("SkyBox", matModel);
    g_pRenderManager->DrawObject("Vampire01", matModel);
    g_pRenderManager->DrawObject("Vampire02", matModel);
    g_pRenderManager->DrawObject("Vampire03", matModel);

    g_pTerrain->RenderTerrain();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}

void cScene05::m_UpdateScreen02()
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_pFBO_2->ID);
    glViewport(0, 0, g_pFBO_2->width, g_pFBO_2->height);
    g_pFBO_2->ClearBuffers();

    glm::vec3 cameraPos = glm::vec3(2723.8f, 52.613f, 2321.85f);
    glm::vec3 lookAt = glm::vec3(2715.14f, 50.53f, 2322.02f);
    glm::vec3 direction = lookAt - cameraPos;

    g_pCamera->SetCameraMod(0);
    g_pCamera->SetBufferSize(g_pFBO_2->width, g_pFBO_2->height);
    g_pCamera->SetDirection(direction);
    g_pCamera->SetPosition(cameraPos);
    g_pCamera->Update(g_ShaderID);

    cObject3D* box = g_pFindMeshByFriendlyName("SkyBox");
    box->SetPosition(cameraPos);

    glm::mat4 matModel = glm::mat4(1.0);
    g_pRenderManager->DrawObject("SkyBox", matModel);
    g_pRenderManager->DrawObject("Warrok01", matModel);
    g_pRenderManager->DrawObject("Warrok02", matModel);
    g_pRenderManager->DrawObject("Warrok03", matModel);

    g_pTerrain->RenderTerrain();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return;
}

void cScene05::m_DisplayScreen01()
{
    if (!m_bScreen01TurnOn)
    {
        pScreen1->bHasColor = true;
        pScreen1->SetColor(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

        return;
    }

    pScreen1->bHasColor = false;

    //uniform bool bIsStaticScreen;
    g_pShaderManager->SetUniform(g_ShaderID, "bIsStaticScreen", true);

    //uniform bool bIsOffScreenTexture;
    g_pShaderManager->SetUniform(g_ShaderID, "bIsOffScreenTexture", true);

    // uniform vec2 screenSize;
    glm::vec2 size = glm::vec2(g_pFBO_1->width, g_pFBO_1->height);
    g_pShaderManager->SetUniform(g_ShaderID, "screenSize", size);

    if (m_bIsAberration01)
    {
        // uniform bool bIsAberration;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsAberration", true);
    }
    if (m_bIsNightVision01)
    {
        // uniform bool bIsNightVision;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsNightVision", true);
    }
    if (m_bIsBlurScreenBasic01)
    {
        // uniform bool bIsBlurScreenBasic;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenBasic", true);
    }
    if (m_bIsBlurScreenSlower01)
    {
        // uniform bool bIsBlurScreenSlower;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenSlower", true);
    }
    if (m_bIsBlurScreenFaster01)
    {
        // uniform bool bIsBlurScreenFaster;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenFaster", true);
    }

    GLint textureUnitNumber = 0;
    GLint texture_off_screen_UL = 0;
    glm::mat4 matModel = glm::mat4(1.0);

    // FBO_01 to texture Screen01
    textureUnitNumber = 98;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, g_pFBO_1->colourTexture_0_ID);

    texture_off_screen_UL = glGetUniformLocation(g_ShaderID, "textureOffScreen");
    glUniform1i(texture_off_screen_UL, textureUnitNumber);

    g_pRenderManager->DrawObject("Screen01", matModel);



    g_pShaderManager->SetUniform(g_ShaderID, "bIsAberration", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsNightVision", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenBasic", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenSlower", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenFaster", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsOffScreenTexture", false);

    return;
}

void cScene05::m_DisplayScreen02()
{
    if (!m_bScreen02TurnOn)
    {
        pScreen2->bHasColor = true;
        pScreen2->SetColor(glm::vec4(0.05f, 0.05f, 0.05f, 1.0f));

        return;
    }

    pScreen2->bHasColor = false;

    //uniform bool bIsStaticScreen;
    g_pShaderManager->SetUniform(g_ShaderID, "bIsStaticScreen", true);

    //uniform bool bIsOffScreenTexture;
    g_pShaderManager->SetUniform(g_ShaderID, "bIsOffScreenTexture", true);

    // uniform vec2 screenSize;
    glm::vec2 size = glm::vec2(g_pFBO_2->width, g_pFBO_2->height);
    g_pShaderManager->SetUniform(g_ShaderID, "screenSize", size);

    if (m_bIsAberration02)
    {
        // uniform bool bIsAberration;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsAberration", true);
    }
    if (m_bIsNightVision02)
    {
        // uniform bool bIsNightVision;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsNightVision", true);
    }
    if (m_bIsBlurScreenBasic02)
    {
        // uniform bool bIsBlurScreenBasic;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenBasic", true);
    }
    if (m_bIsBlurScreenSlower02)
    {
        // uniform bool bIsBlurScreenSlower;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenSlower", true);
    }
    if (m_bIsBlurScreenFaster02)
    {
        // uniform bool bIsBlurScreenFaster;
        g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenFaster", true);
    }

    GLint textureUnitNumber = 0;
    GLint texture_off_screen_UL = 0;
    glm::mat4 matModel = glm::mat4(1.0);

    // FBO_02 to texture Screen02
    textureUnitNumber = 99;
    glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
    glBindTexture(GL_TEXTURE_2D, g_pFBO_2->colourTexture_0_ID);

    texture_off_screen_UL = glGetUniformLocation(g_ShaderID, "textureOffScreen");
    glUniform1i(texture_off_screen_UL, textureUnitNumber);
    g_pRenderManager->DrawObject("Screen02", matModel);

    glBindTexture(GL_TEXTURE_2D, 0);

    g_pShaderManager->SetUniform(g_ShaderID, "bIsAberration", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsNightVision", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenBasic", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenSlower", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsBlurScreenFaster", false);
    g_pShaderManager->SetUniform(g_ShaderID, "bIsOffScreenTexture", false);

    return;
}