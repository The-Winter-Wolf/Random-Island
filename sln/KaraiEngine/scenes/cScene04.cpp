#include "cScene04.h"

#include "../global/global.h"

#include "../objects/object_3D/cObject3D.h"
extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyNameToFind);

#include "../world/player/cPlayer.h"
cPlayer* g_pPlayer = nullptr;

#include "../graphics/textures/cTextureManager.h"
extern cTextureManager* g_pTextureManager;

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../physics/cPhysicsManager.h";
extern cPhysicsManager* g_pPhysicsManager;

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../utils/cRandomUtils.h"
extern cRandomUtils* g_pRandUtils;

#include <iostream>

// The number of threads is configured in the header file, there
#include "../system/threads/cThreadAnimations.h"
extern cThreadAnimations* g_pThreadAnimations;

// The number of enemies and, accordingly, 
// the number of animations not including the player animation
const unsigned int cScene04::NUM_OF_ENEMIES = 4;

cScene04::cScene04() 
{
    this->m_Window = nullptr;
}

cScene04::~cScene04() {}

void cScene04::Init(GLFWwindow* window)
{
    this->m_Window = window;

    this->m_HandLoad();

    this->m_InitPlayer();

    this->m_InitEnemies();

    // Comment here for one thread animations
    g_pThreadAnimations->InitThreads();
}

void cScene04::Update(float deltaTime)
{
    this->m_UpdateCamera();

    g_pPlayer->UpdatePlayer(deltaTime);

    for (int index = 0; index < m_vpEnemies.size(); index++)
    {
        m_vpEnemies[index]->UpdateEnemy(deltaTime);
    }

    // Comment here for one thread animations
    g_pThreadAnimations->UpdateThreads(deltaTime);

    // Comment here for multi thread animations
    //g_pThreadAnimations->UpdateAnimators(deltaTime);
}

void cScene04::InputKey(int event)
{
    if (event == 0)
    {
        return;
    }
    if (event == 1)
    {
        return;
    }
    if (event == 2)
    {
        return;
    }
    if (event == 3)
    {
        return;
    }

    return;
}

bool cScene04::m_HandLoad()
{
    return true;
}

bool cScene04::m_InitPlayer()
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

    return true;
}

bool cScene04::m_InitEnemies()
{
    m_vpEnemies.reserve(NUM_OF_ENEMIES);

    g_pModelManager->SetBasePath("../../extern/assets/models/warrok");
    cModel* model = g_pModelManager->LoadModelIntoVAO("Unarmed Walk Forward.dae", g_ShaderID);

    for (unsigned int index = 0; index < NUM_OF_ENEMIES; index++)
    {
        cObject3D* enemy = new cObject3D();
        enemy->BindModelToObject(model);
        enemy->friendlyName = "Warrok" + std::to_string(index);

        glm::vec3 pos(0.0f);
        pos.x = g_pRandUtils->GetRandomFloat(1050, 1150);
        pos.z = g_pRandUtils->GetRandomFloat(1050, 1150);
        enemy->SetPosition(pos);

        enemy->bHasLight = true;
        enemy->bHasColor = false;
        enemy->bIsVisible = true;
        enemy->bIsTwoSided = true;

        enemy->GetPhysBody()->SetRigidBody(cPhysicsBody::DYNAMIC);
        enemy->GetPhysBody()->SetAutoSlowdown(0.9f, 1.0f, 0.9f);
        enemy->GetPhysBody()->SetSphereCollider(1.0f);
        enemy->GetPhysBody()->SetRestitution(0.0f);
        enemy->GetPhysBody()->SetMass(0.1f);

        g_pPhysicsManager->AddPhysObject(enemy);
        g_vpObjectToDraw.push_back(enemy);

        cBigEnemy* bigEnemy = new cBigEnemy();
        bigEnemy->InitEnemy(enemy, g_pRandUtils->GetRandomFloat(1, 1));
        m_vpEnemies.push_back(bigEnemy);
    }

    return true;
}

bool cScene04::m_UpdateCamera()
{
    int width = 0, height = 0;

    glfwGetFramebufferSize(m_Window, &width, &height);

    glViewport(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);

    glDisable(GL_BLEND);

    g_pCamera->SetBufferSize(width, height);

    g_pCamera->Update(g_ShaderID);

    return true;
}
