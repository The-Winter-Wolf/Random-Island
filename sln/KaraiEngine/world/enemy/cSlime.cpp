#include "cSlime.h"

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyName);

#include "../../physics/cPhysicsManager.h"
extern cPhysicsManager* g_pPhysicsManager;

#include <glm/gtx/vector_angle.hpp>

#include "../../global/global.h"

#include "../../utils/cRandomUtils.h"
extern cRandomUtils* g_pRandUtils;

#include "../player/cPlayer.h"
extern cPlayer* g_pPlayer;

cSlime::cSlime(unsigned int index)
{
    this->bIsTrigger1 = false;
    this->bIsTrigger2 = false;
    this->m_triggerRadius1 = 5.0f;
    this->m_triggerRadius2 = 6.0f;

    this->m_enemyIndex = index;
    this->m_pObject = nullptr;
    this->m_pBehaviour = nullptr;
}
cSlime::~cSlime() {}

void cSlime::InitEnemy()
{
    // Init Object
    this->m_InitObject();

    // Init Behaviour
    this->m_InitBehaviour();
}
void cSlime::UpdateEnemy(float deltaTime)
{
    this->m_TriggerEvents(deltaTime);

    this->m_pBehaviour->UpdateBehaviour(deltaTime);
}

cObject3D* cSlime::GetEnemyObject()
{
    return this->m_pObject;
}
unsigned int cSlime::GetEnemyIndex()
{
    return this->m_enemyIndex;
}

void cSlime::SetBehaviour(cBehaviour* behaviour)
{
    this->m_pBehaviour = behaviour;
}
cBehaviour* cSlime::GetBehaviour() const
{
    return this->m_pBehaviour;
}

void cSlime::m_InitObject()
{
    this->m_pObject = g_pFindMeshByFriendlyName("Slime" + std::to_string(m_enemyIndex));

    if (m_pObject)
    {
        return;
    }

    this->m_pObject = new cObject3D();
    this->m_pObject->friendlyName = "Slime" + std::to_string(m_enemyIndex);
    this->m_pObject->SetOrientationFromEuler(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
    this->m_pObject->SetOpacity(0.6f);
    this->m_pObject->bHasLight = true;
    this->m_pObject->bHasColor = true;
    this->m_pObject->bIsVisible = true;
    this->m_pObject->bIsTwoSided = true;
    this->m_pObject->bIsWireframe = false;

    float r = g_pRandUtils->GetRandomFloat(0.0f, 255.0f) / 255.0f;
    float g = g_pRandUtils->GetRandomFloat(0.0f, 255.0f) / 255.0f;
    float b = g_pRandUtils->GetRandomFloat(0.0f, 255.0f) / 255.0f;
    this->m_pObject->SetColor(glm::vec4(r, g, b, 1.0f));

    float posX = g_pRandUtils->GetRandomFloat(-30.0f, 30.0f) + WORLD_POS.x;
    float posZ = g_pRandUtils->GetRandomFloat(-30.0f, 30.0f) + WORLD_POS.z;
    float posY = g_pRandUtils->GetRandomFloat(5.0f, 10.0f);
    this->m_pObject->SetPosition(glm::vec3(posX, posY, posZ));

    float scale = g_pRandUtils->GetRandomFloat(1000.0f, 2500.0f) / 1000.0f;
    this->m_pObject->SetScale(glm::vec3(scale));

    g_vpObjectToDraw.push_back(m_pObject);
     
    // Init Properties
    sGameplay* gameComp = this->m_pObject->GetGameplayComponent();
    gameComp->bImmortal = true;
    gameComp->bIsDead = false;
    gameComp->bIsHit = false;
    gameComp->bIsPlayer = false;
    gameComp->Damage = 50.0f;
    gameComp->Exp = 1000.0f;
    gameComp->Health = 100.0f;
    gameComp->Mana = 100.0f;
    gameComp->Stamina = 100.0f;

    this->m_pObject->BindModelToObject(m_pObject->GetPhysBody()->
        SetSoftBody("Sphere_r1_xyz_n_rgba.ply", m_pObject->GetMatModel()));
    this->m_pObject->GetPhysBody()->SetSphereCollider(0.5f);
    this->m_pObject->GetPhysBody()->SetMass(0.1);
    float restitution = 1.0f/*static_cast<float>(rand() % 900) / 1000.0f + 0.1f*/;

    this->m_pObject->GetPhysBody()->SetRestitution(restitution);
    this->m_pObject->GetPhysBody()->GetSoftBody()->bIsConvert = true;
    this->m_pObject->GetPhysBody()->GetSoftBody()->CreateRandomBracing(100/*(rand() % 100)*/, 1.0f);
    this->m_pObject->GetPhysBody()->SetAutoSlowdown(0.99f, 0.99f, 0.99f);
    g_pPhysicsManager->AddPhysObject(m_pObject);
    g_vpObjectToDraw.push_back(m_pObject);
}
void cSlime::m_InitBehaviour()
{
    // Some Default Behaviour
    int numInit = 0;

    if (numInit == 1)
    {
        m_pObject->bHasColor = false;
        //m_pObject->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 10.0f, 1.0f);
    }
    else if (numInit == 2)
    {
        m_pObject->bHasColor = false;
        //m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 20.0f, 1.0f);
    }
    else if (numInit == 3)
    {
        m_pObject->bHasColor = false;
        //m_pObject->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 30.0f, 1.0f);
    }
    else if (numInit == 4)
    {
        m_pObject->bHasColor = false;
        //m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderRadiusBehaviour(m_pObject, 10.0f, 2.0f, 20.0f, 2.0f);
    }
    else if (numInit == 5)
    {
        m_pObject->bHasColor = false;
        //m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.SeekBehaviour(m_pObject, g_pPlayer->GetPlayerObject(), 5.0f, 5.0f, 3.0f);
    }
}

void cSlime::m_TriggerEvents(float dt)
{
    float distance = glm::distance(g_pPlayer->GetPlayerPos(), m_pObject->GetPosition());
    if (distance < 1.0f)
    {
        m_pObject->GetGameplayComponent()->bIsHit = true;
    }
    if (distance >= m_triggerRadius2)
    {
        this->m_pBehaviour->PlayBehaviour("Path");
        this->bIsTrigger1 = false;
        this->bIsTrigger2 = false;
    }
    else if (distance < m_triggerRadius1 || bIsTrigger1)
    {
        this->m_pBehaviour->PlayBehaviour("Seek");
        this->bIsTrigger1 = true;
    }
    else if (distance < m_triggerRadius2)
    {
        this->m_pBehaviour->PlayBehaviour("Look");
        this->bIsTrigger2 = true;
    }
}

