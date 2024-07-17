#include "cVampire.h"

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyName);

#include "../../system/threads/cThreadAnimations.h"
extern cThreadAnimations* g_pThreadAnimations;

#include "../../physics/cPhysicsManager.h"
extern cPhysicsManager* g_pPhysicsManager;

#include "../../animations/cAnimator.h"

#include <glm/gtx/vector_angle.hpp>

#include "../../global/global.h"

#include "../player/cPlayer.h"
extern cPlayer* g_pPlayer;

cVampire::cVampire(unsigned int index)
{
    this->bIsTrigger1 = false;
    this->bIsTrigger2 = false;
    this->m_triggerRadius1 = 10.0f;
    this->m_triggerRadius2 = 15.0f;

    this->m_enemyIndex = index;
    this->m_pObject = nullptr;
    this->m_pBehaviour = nullptr;
}

cVampire::~cVampire() {}

void cVampire::InitEnemy()
{
    // Init Object
    this->m_InitObject();

    // Init Animations
    this->m_InitAnimations();

    // Init Behaviour
    this->m_InitBehaviour();
}

void cVampire::UpdateEnemy(float deltaTime)
{
    this->m_CheckFieldOfView();

    this->m_TriggerEvents(deltaTime);

    this->m_pBehaviour->UpdateBehaviour(deltaTime);
}

cObject3D* cVampire::GetEnemyObject()
{
    return this->m_pObject;
}

unsigned int cVampire::GetEnemyIndex()
{
    return this->m_enemyIndex;
}

void cVampire::SetBehaviour(cBehaviour* behaviour)
{
    this->m_pBehaviour = behaviour;
}
cBehaviour* cVampire::GetBehaviour() const
{
    return this->m_pBehaviour;
}

void cVampire::m_InitObject()
{
    this->m_pObject = g_pFindMeshByFriendlyName("Vampire" + std::to_string(m_enemyIndex));

    if (!m_pObject)
    {
        this->m_pObject = new cObject3D();
        this->m_pObject->BindModelToObject("Breathing Idle.dae");
        this->m_pObject->friendlyName = "Vampire" + std::to_string(m_enemyIndex);
        this->m_pObject->SetOrientationFromEuler(glm::radians(glm::vec3(0.0f, 0.0f, 0.0f)));
        this->m_pObject->SetPosition(glm::vec3(125.0f, 0.0f, 125.0f));
        this->m_pObject->SetScale(glm::vec3(0.01f, 0.01f, 0.01f));
        this->m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        this->m_pObject->SetOpacity(1.0f);
        this->m_pObject->bHasLight = true;
        this->m_pObject->bHasColor = false;
        this->m_pObject->bIsVisible = true;
        this->m_pObject->bIsTwoSided = true;
        this->m_pObject->bIsWireframe = false;
        g_vpObjectToDraw.push_back(m_pObject);

        // Init Properties
        sGameplay* gameComp = this->m_pObject->GetGameplayComponent();
        gameComp->bImmortal = false;
        gameComp->bIsDead = false;
        gameComp->bIsHit = false;
        gameComp->bIsPlayer = false;
        gameComp->Damage = 10.0f;
        gameComp->Exp = 100.0f;
        gameComp->Health = 100.0f;
        gameComp->Mana = 100.0f;
        gameComp->Stamina = 100.0f;
    }

    this->m_pObject->GetPhysBody()->SetRigidBody(cPhysicsBody::DYNAMIC);
    this->m_pObject->GetPhysBody()->SetAutoSlowdown(0.9f, 0.9f, 0.9f);
    this->m_pObject->GetPhysBody()->SetSphereCollider(1.0f);
    this->m_pObject->GetPhysBody()->SetRestitution(0.0f);
    this->m_pObject->GetPhysBody()->SetMass(0.1f);
    g_pPhysicsManager->AddPhysObject(m_pObject);
}

void cVampire::m_InitBehaviour()
{
    // Some Default Behaviour
    int numInit = 0;

    if (numInit == 1)
    {
        m_pObject->bHasColor = true;
        //m_pObject->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 10.0f, 1.0f);
    }
    else if (numInit == 2)
    {
        m_pObject->bHasColor = true;
        //m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 20.0f, 1.0f);
    }
    else if (numInit == 3)
    {
        m_pObject->bHasColor = true;
        //m_pObject->SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderFreeBehaviour(m_pObject, 2.0f, 5.0f, 20.0f, 30.0f, 1.0f);
    }
    else if (numInit == 4)
    {
        m_pObject->bHasColor = true;
        //m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.WanderRadiusBehaviour(m_pObject, 10.0f, 2.0f, 20.0f, 2.0f);
    }
    else if (numInit == 5)
    {
        m_pObject->bHasColor = true;
        //m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
        m_pObject->SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //behaviour.SeekBehaviour(m_pObject, g_pPlayer->GetPlayerObject(), 5.0f, 5.0f, 3.0f);
    }
}

void cVampire::m_InitAnimations()
{
    cAnimation* pAnimation4 = new cAnimation();
    pAnimation4->SetBasePath("../../extern/assets/animations/vampire");
    pAnimation4->LoadAnimationFromFile("Running.dae", m_pObject->GetModel());

    cAnimation* pAnimation5 = new cAnimation();
    pAnimation5->SetBasePath("../../extern/assets/models/vampire");
    pAnimation5->LoadAnimationFromFile("Breathing Idle.dae", m_pObject->GetModel());

    cAnimation* pAnimation6 = new cAnimation();
    pAnimation6->SetBasePath("../../extern/assets/animations/vampire");
    pAnimation6->LoadAnimationFromFile("Mma_Kick.dae", m_pObject->GetModel());

    cAnimator* pAnimator = new cAnimator();
    pAnimator->AddAnimation(pAnimation4, walk_forward);
    pAnimator->AddAnimation(pAnimation5, idle);
    pAnimator->AddAnimation(pAnimation6, kick);

    this->m_pObject->SetAnimator(pAnimator);
    this->m_pObject->GetAnimator()->PlayAnimation(idle, 2.0f);

    g_pThreadAnimations->AddAnimator(pAnimator);
}

void cVampire::m_TriggerEvents(float dt)
{
    float distance = glm::distance(g_pPlayer->GetPlayerPos(), m_pObject->GetPosition());

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

    if (m_CheckFieldOfView())
    {
        this->m_pObject->GetAnimator()->PlayAnimation(kick, 3.0f);
        if (this->m_pObject->GetAnimator()->GetAnimTrigger(kick, 50.0f))
        {
            m_pObject->GetGameplayComponent()->bIsHit = true;
        }
    }
    else if (bIsTrigger1)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(walk_forward, 2.0f);
    }
    else if (bIsTrigger2)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(idle, 2.0f);
    }
    else
    {
        this->m_pObject->GetAnimator()->PlayAnimation(walk_forward, 2.0f);
    }
}

bool cVampire::m_CheckFieldOfView()
{
    if (glm::distance(m_pObject->GetPosition(), g_pPlayer->GetPlayerObject()->GetPosition()) > 3.0f)
    {
        return false;
    }

    glm::vec3 goalDir = g_pPlayer->GetPlayerObject()->GetPosition() - m_pObject->GetPosition();
    goalDir = glm::normalize(goalDir);

    if (glm::angle(goalDir, m_pObject->GetDirection()) > 0.5f)
    {
        return false;
    }

    return true;
}