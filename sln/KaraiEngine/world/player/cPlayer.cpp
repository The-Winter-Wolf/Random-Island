 #include "cPlayer.h"

#include "../../global/global.h"
#include "../../system/threads/cThreadAnimations.h"
extern cThreadAnimations* g_pThreadAnimations;

#include "../../animations/cAnimator.h"
#include "../../physics/cPhysicsManager.h"
extern cPhysicsManager* g_pPhysicsManager;

extern cObject3D* g_pFindMeshByFriendlyName(std::string friendlyName);

cPlayer::cPlayer()
{
    this->bIsLeft = false;
    this->bIsRight = false;
    this->bIsJump = false;
    this->bIsSquat = false;
    this->bIsAttack = false;
    this->bIsLeftAttack = false;
    this->bIsRightAttack = false;
    this->bIsDash = false;
    this->bIsForward = false;
    this->bIsBackward = false;

    this->bHasAnimations = true;
    this->bJumpMovements = false;

    this->m_maxSpeed = 10.0f;
    this->m_moveSpeed = 100.0f;
    this->m_jumpSpeed = 500.0f;
    this->m_squatSpeed = 5.0f;
    this->m_boostSpeed = 200.0f;
    this->m_playerPos = glm::vec3(0.0f);

    this->m_pObject = nullptr;

    this->bUseFear = false;
    this->m_speed = 0.0f;
    this->m_jumpTime = 0.0f;
}

cPlayer::~cPlayer(){}

void cPlayer::InitPlayer()
{
    // Init Object
    this->m_pObject = g_pFindMeshByFriendlyName("Paladin");
    this->m_pObject->GetPhysBody()->SetRigidBody(cPhysicsBody::DYNAMIC);
    this->m_pObject->GetPhysBody()->SetSphereCollider(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    this->m_pObject->GetPhysBody()->SetRestitution(0.0f);
    this->m_pObject->GetPhysBody()->SetMass(0.1f);
    g_pPhysicsManager->AddPhysObject(this->m_pObject);

    // Init Properties
    sGameplay* gameComp = this->m_pObject->GetGameplayComponent();
    gameComp->bImmortal = false;
    gameComp->bIsDead = false;
    gameComp->bIsHit = false;
    gameComp->bIsPlayer = true;
    gameComp->Damage = 50.0f;
    gameComp->Exp = 0.0f;
    gameComp->Health = 100.0f;
    gameComp->Mana = 100.0f;
    gameComp->Stamina = 100.0f;

    // Init Animations
    this->m_InitAnimation();
}

void cPlayer::UpdatePlayer(float deltaTime)
{    
    this->m_UpdateAnimation(deltaTime);
    
    this->m_Slowdown(deltaTime);
}

void cPlayer::MovePlayer(ePlayerEvents event, float eventValue)
{ 
    if (event == PLAYER_BOOST && eventValue == 0)
    {
        m_speed = m_moveSpeed;
        return;
    }
    if (event == PLAYER_BOOST && eventValue == 1)
    {
        m_speed = m_boostSpeed;
        return;
    }
    //if (event == PLAYER_SQUAT && eventValue == 1)
    //{
    //    m_speed = m_squatSpeed;
    //    return;
    //}

    glm::vec3 acceleration = glm::vec3(0.0f);

    if (event == PLAYER_FORWARD && (!bIsJump || bJumpMovements))
    {
        acceleration = m_speed * m_pObject->GetDirection();
        m_pObject->GetPhysBody()->AddUserAccel(acceleration);
        bIsForward = true;    
        return;
    }
    if (event == PLAYER_BACKWARD && (!bIsJump || bJumpMovements))
    {
        acceleration = -m_speed * m_pObject->GetDirection();
        m_pObject->GetPhysBody()->AddUserAccel(acceleration);
        bIsBackward = true;
        return;
    }
    if (event == PLAYER_LEFT && (!bIsJump || bJumpMovements))
    {
        acceleration = m_speed * m_pObject->GetRight();
        m_pObject->GetPhysBody()->AddUserAccel(acceleration);
        bIsLeft = true;
        return;
    }
    if (event == PLAYER_RIGHT && (!bIsJump || bJumpMovements))
    {
        acceleration = -m_speed * m_pObject->GetRight();
        m_pObject->GetPhysBody()->AddUserAccel(acceleration);
        bIsRight = true;
        return;
    }

    if (event == PLAYER_JUMP && !bIsJump)
    {
        glm::vec3 acceleration = m_jumpSpeed * m_pObject->GetUp();
        m_pObject->GetPhysBody()->AddUserAccel(acceleration);       
        bIsJump = true;
        return;
    }

    if (event == PLAYER_LEFT_ATTACK)
    {
        if (eventValue == 1)
        {
            bIsLeftAttack = true;
            bIsMouseLock = true;
        }
        else
        {
            bIsLeftAttack = false;
        }
        return;
    }

    if (event == PLAYER_RIGHT_ATTACK)
    {
        if (eventValue == 1)
        {
            bIsRightAttack = true;
            bIsMoveLock = true;
            bIsMouseLock = true;
            bUseFear = true;
        }
        else
        {
            bIsRightAttack = false;
        }
        return;
    }
}

void cPlayer::SetPlayerObject(cObject3D* playerObj)
{
    this->m_pObject = playerObj;
}
cObject3D* cPlayer::GetPlayerObject()
{
    return this->m_pObject;
}

glm::vec3 cPlayer::GetPlayerPos()
{
    m_playerPos = m_pObject->GetPosition();
    return this->m_playerPos;
}

void cPlayer::m_Slowdown(float deltaTime)
{   
    //m_jumpTime -= deltaTime;
    //if (m_jumpTime < 0.0f && m_pPlayerObject->GetPhysBody()->GetOnTheGroundFlag())
    //{
    //    m_jumpTime = 0.0f;
    //    bIsJump = false;
    //}  

    glm::vec3 velocity = m_pObject->GetPhysBody()->GetVelocity();
    glm::vec2 velXZ = glm::vec2(velocity.x, velocity.z);
    float length = glm::length(velXZ);

    if (length > m_maxSpeed)
    {
        m_pObject->GetPhysBody()->Slowdown(0.9f, 1.0f, 0.9f);
    }

    if (m_pObject->GetPhysBody()->GetOnTheGroundFlag())
    {
        if (!bIsJump && !bIsBackward && !bIsForward && !bIsLeft && !bIsRight)
        {
            m_pObject->GetPhysBody()->Slowdown(0.5f, 0.9f, 0.5f);
        }
    }

    bIsForward = false;
    bIsBackward = false;
    bIsLeft = false;
    bIsRight = false;
}

void cPlayer::m_InitAnimation()
{
    if (!bHasAnimations)
    {
        return;
    }

    cAnimation* pAnimation1 = new cAnimation();
    pAnimation1->SetBasePath("../../extern/assets/models/paladin");
    pAnimation1->LoadAnimationFromFile("Sword_And_Shield_Idle.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation2 = new cAnimation();
    pAnimation2->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation2->LoadAnimationFromFile("Sword_And_Shield_Run_Forward.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation3 = new cAnimation();
    pAnimation3->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation3->LoadAnimationFromFile("Sword_And_Shield_Run_Backward.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation4 = new cAnimation();
    pAnimation4->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation4->LoadAnimationFromFile("Sword_And_Shield_Strafe_Left.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation5 = new cAnimation();
    pAnimation5->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation5->LoadAnimationFromFile("Sword_And_Shield_Strafe_Right.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation6 = new cAnimation();
    pAnimation6->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation6->LoadAnimationFromFile("Sword_And_Shield_Jump.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation7 = new cAnimation();
    pAnimation7->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation7->LoadAnimationFromFile("Sword_And_Shield_Slash.dae", this->m_pObject->GetModel());

    cAnimation* pAnimation8 = new cAnimation();
    pAnimation8->SetBasePath("../../extern/assets/animations/paladin");
    pAnimation8->LoadAnimationFromFile("Sword_And_Shield_Power_Up.dae", this->m_pObject->GetModel());

    cAnimator* pPlayerAnimator = new cAnimator();
    pPlayerAnimator->AddAnimation(pAnimation1, idle);
    pPlayerAnimator->AddAnimation(pAnimation2, run_forward);
    pPlayerAnimator->AddAnimation(pAnimation3, run_backward);
    pPlayerAnimator->AddAnimation(pAnimation4, run_left);
    pPlayerAnimator->AddAnimation(pAnimation5, run_right);
    pPlayerAnimator->AddAnimation(pAnimation6, jump_run);
    pPlayerAnimator->AddAnimation(pAnimation7, slash);
    pPlayerAnimator->AddAnimation(pAnimation8, power_up);

    this->m_pObject->SetAnimator(pPlayerAnimator);
    this->m_pObject->GetAnimator()->PlayAnimation(idle, 2.0f);

    g_pThreadAnimations->AddAnimator(pPlayerAnimator);
}

void cPlayer::m_UpdateAnimation(float deltaTime)
{
    if (!bHasAnimations)
    {
        return;
    }

    if (bIsRightAttack && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(power_up, 4.0f);
        this->m_pObject->GetAnimator()->SetAnimLock(true);
        if (this->m_pObject->GetAnimator()->GetFinishedFlag())
        {
            this->m_pObject->GetAnimator()->SetAnimLock(false);
            bIsRightAttack = false;
            bIsMouseLock = false;
            bIsMoveLock = false;
        }
    }
    else if (bIsLeftAttack && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(slash, 4.0f);
        this->m_pObject->GetAnimator()->SetAnimLock(true);
        if (this->m_pObject->GetAnimator()->GetFinishedFlag())
        {
            this->m_pObject->GetAnimator()->SetAnimLock(false);
            bIsLeftAttack = false;
            bIsMouseLock = false;
            bIsMoveLock = false;
        }

        if (this->m_pObject->GetAnimator()->GetAnimTrigger(slash, 50.0f))
        {
            m_pObject->GetGameplayComponent()->bIsHit = true;
        }
    }
    else if (bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(jump_run , 2.0f);
        this->m_pObject->GetAnimator()->SetAnimLock(true);
        if (this->m_pObject->GetAnimator()->GetFinishedFlag())
        {
            this->m_pObject->GetAnimator()->SetAnimLock(false);
            bIsJump = false;
        }
    }
    else if (bIsForward && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(run_forward, 2.0f);
    }
    else if (bIsBackward && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(run_backward, 2.0f);
    }
    else if (bIsLeft && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(run_left, 2.0f);
    }
    else if (bIsRight && !bIsJump)
    {
        this->m_pObject->GetAnimator()->PlayAnimation(run_right, 2.0f);
    }
    else
    {
        this->m_pObject->GetAnimator()->PlayAnimation(idle, 2.0f);
    }
}
