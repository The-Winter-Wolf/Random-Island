#include "cAnimator.h"
#include "../utils/cMaths.h"
extern cMaths* g_pMaths;

cAnimator::cAnimator()
{
    this->bIsRunning = false;
    this->bIsFinished = false;
    this->bIsAnimChanging = false;

    this->m_timer = 100.0f;
    this->m_deltaTime = 0.0f;
    this->m_speedFactor = 2.0f;

    this->m_baseCurrentTime = 0.0f;
    this->m_changingTime = 0.3f;

    this->m_baseAnimation = nullptr;
    this->m_goalAnimation = nullptr;

    this->m_vecFinalBoneMatrices.reserve(100);
    
    for (int i = 0; i < 100; i++)
    {
        this->m_vecFinalBoneMatrices.push_back(glm::mat4(1.0f));
    }
}

cAnimator::~cAnimator()
{
    m_animIterator = m_mapAnimations.begin();
    for (m_animIterator; m_animIterator != m_mapAnimations.end(); m_animIterator++)
    {
        delete m_animIterator->second;
        m_mapAnimations.erase(m_animIterator);
        m_animIterator--;
    }
}

void cAnimator::AddAnimation(cAnimation* pAnimation, const eAnimStates& animState)
{
    this->m_mapAnimations[animState] = pAnimation;
}

void cAnimator::DeleteAnimation(const eAnimStates& animState)
{
    m_animIterator = m_mapAnimations.find(animState);
    if (m_animIterator != m_mapAnimations.end())
    {
        delete m_animIterator->second;       
        m_mapAnimations.erase(animState);
    }
    return;
}

void cAnimator::PlayAnimation(const eAnimStates& animState, float speed)
{
    if (bIsLocked)
    {
        return;
    }

    m_animIterator = m_mapAnimations.find(animState);
    if (m_animIterator == m_mapAnimations.end())
    {
        return;
    }

    m_speedFactor = speed;

    if (m_goalAnimation == nullptr)
    {
        this->m_goalAnimation = m_animIterator->second;
        this->bIsAnimChanging = false;
        this->bIsRunning = true;
        return;
    }

    if (m_goalAnimation == m_animIterator->second)
    {
        this->bIsAnimChanging = false;
    }
    else
    {
        this->m_baseAnimation = m_goalAnimation;
        this->m_goalAnimation = m_animIterator->second;
        this->bIsAnimChanging = true;
        this->m_goalCurrentTime = 0.0f;
        this->m_timer = 0.0f;
    }

    this->bIsRunning = true;

    return;
}

void cAnimator::UpdateAnimation(float dt)
{
    if (!bIsRunning)
    {
        return;
    }

    m_deltaTime = dt * m_speedFactor;

    if (bIsFinished)
    {
        bIsFinished = false;
        bIsLocked = false;
    }

    //float m_timer = m_AnimState.GetState(m_goalAnimState, m_baseAnimState, m_nextAnimState);

    //m_animIterator = m_mapAnimations.find(m_baseAnimState)

    if (m_changingTime > m_timer)
    {

        m_blendFactor = m_timer / m_changingTime;

        this->m_BlendAnimations(m_baseAnimation, m_goalAnimation); 

        m_timer += m_deltaTime;
    }
    else
    {
        this->m_UpdateBoneMatrices();
    }
}

std::vector<glm::mat4> cAnimator::GetFinalBoneMatrices()
{
    return this->m_vecFinalBoneMatrices;
}

float cAnimator::GetCurrentTime()
{
    return m_goalCurrentTime;
}

float cAnimator::GetDuration()
{
    return m_goalAnimation->GetDuration();
}

bool cAnimator::GetFinishedFlag()
{
    return bIsFinished;
}

bool cAnimator::GetRunningFlag()
{
    return this->bIsRunning;
}

void cAnimator::SetAnimLock(bool true_false)
{
    this->bIsLocked = true_false;
}

bool cAnimator::GetAnimLock()
{
    return this->bIsLocked;
}

void cAnimator::ResetTime()
{
    m_goalCurrentTime = 0.0f;
}

void cAnimator::IsRunning(bool true_false)
{
    if (true_false)
    {
        this->bIsRunning = true;
    }
    else
    {
        //this->ResetTime();
        this->bIsRunning = false;
    }

    return;
}

bool cAnimator::GetAnimTrigger(eAnimStates state, float procentTime)
{
    m_animIterator = m_mapAnimations.find(state);
    if (m_animIterator == m_mapAnimations.end())
    {
        return false;
    }

    if (m_goalAnimation != m_animIterator->second)
    {
        return false;
    }

    float duration = m_animIterator->second->GetDuration();
    float deltaTime = duration * m_deltaTime;
    float goalTime = duration * procentTime / 100.0f;
    if (m_goalCurrentTime < goalTime)
    {
        bIsHit = false;
    }
    else if (m_goalCurrentTime >= goalTime && ! bIsHit)
    {
        bIsHit = true;
        return true;
    }
    return false;
}

void cAnimator::m_BlendAnimations(cAnimation* baseAnimation, cAnimation* goalAnimation)
{
    float a = 1.0f;
    float b = m_baseAnimation->GetDuration() / m_goalAnimation->GetDuration();
    float animSpeedUp = glm::mix(a, b, m_blendFactor);

    a = m_goalAnimation->GetDuration() / m_baseAnimation->GetDuration();
    b = 1.0f;
    float animSpeedDown = glm::mix(a, b, m_blendFactor);

    m_baseCurrentTime = 0.0f;
    m_baseCurrentTime += m_baseAnimation->GetTicksPerSecond() * m_deltaTime * animSpeedUp;
    float tempTime = fmod(m_baseCurrentTime, m_baseAnimation->GetDuration());

    if (m_baseCurrentTime > tempTime) bIsFinished = true;
    m_baseCurrentTime = tempTime;

    m_goalCurrentTime = 0.0f;
    m_goalCurrentTime += m_goalAnimation->GetTicksPerSecond() * m_deltaTime * animSpeedDown;
    tempTime = fmod(m_goalCurrentTime, m_goalAnimation->GetDuration());

    if (m_goalCurrentTime > tempTime) bIsFinished = true;
    m_goalCurrentTime = tempTime;

    this->m_CalculateBlendedBoneTransform(&m_baseAnimation->GetRootNode(), &m_goalAnimation->GetRootNode(), glm::mat4(1.0f));
}

void cAnimator::m_ChangeAnimation()
{
    if (bIsAnimChanging)
    {
        m_vecPastBoneMatrices.clear();
        m_vecPastBoneMatrices = m_vecFinalBoneMatrices;
        this->m_UpdateBoneMatrices();
        m_vecFutureBoneMatrices.clear();
        m_vecFutureBoneMatrices = m_vecFinalBoneMatrices;
        bIsAnimChanging = false;
    }

    m_timer += m_deltaTime;
    float scaleFactor = m_timer / m_changingTime;

    for (int i = 0; i < 100; i++)
    {
        glm::mat4 pastMatrix = m_vecPastBoneMatrices[i];
        glm::mat4 futureMatrix = m_vecFutureBoneMatrices[i];

        glm::vec3 pastPos, futurePos;
        glm::vec3 pastScale, futureScale;
        glm::quat pastRot, futureRot;

        g_pMaths->DisassembleMatrix4(pastMatrix, pastPos, pastScale, pastRot);
        g_pMaths->DisassembleMatrix4(futureMatrix, futurePos, futureScale, futureRot);

        const glm::quat rot0 = glm::quat_cast(pastMatrix);
        const glm::quat rot1 = glm::quat_cast(futureMatrix);

        glm::vec3 finalPosition = glm::mix(pastPos, futurePos, scaleFactor);
        glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f), finalPosition);

        glm::quat finalRotation = glm::slerp(pastRot, futureRot, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        glm::mat4 matRotate = glm::toMat4(finalRotation);
        glm::mat4 matRotater = glm::mat4_cast(finalRotation);

        glm::vec3 finalScale = glm::mix(pastScale, futureScale, scaleFactor);
        glm::mat4 matScale = glm::scale(glm::mat4(1.0f), finalScale);

        m_vecFinalBoneMatrices[i] = matTranslate * matRotate * matScale;
    }
}

void cAnimator::m_UpdateBoneMatrices()
{
    m_goalCurrentTime += m_goalAnimation->GetTicksPerSecond() * m_deltaTime;
    float tempTime = fmod(m_goalCurrentTime, m_goalAnimation->GetDuration());

    if (m_goalCurrentTime > tempTime) bIsFinished = true;
    m_goalCurrentTime = tempTime;

    m_CalculateBoneTransform(&m_goalAnimation->GetRootNode(), glm::mat4(1.0f));
}

void cAnimator::m_CalculateBoneTransform(const sAssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    cBone* pBone = m_goalAnimation->FindBone(nodeName);

    if (pBone != nullptr)
    {
        pBone->Update(m_goalCurrentTime);
        nodeTransform = pBone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    std::map<std::string, sBoneInfo> boneInfoMap = m_goalAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        m_vecFinalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
    {
        m_CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}

void cAnimator::m_CalculateBlendedBoneTransform(const sAssimpNodeData* baseNode, const sAssimpNodeData* goalNode, const glm::mat4& parentTransform)
{
    std::string nodeName = baseNode->name;

    // Update base bone
    glm::mat4 baseNodeTransform = baseNode->transformation;
    cBone* pBone = m_baseAnimation->FindBone(nodeName);
    if (pBone)
    {
        pBone->Update(m_baseCurrentTime);
        baseNodeTransform = pBone->GetLocalTransform();
    }

    // Update goal bone
    glm::mat4 goalNodeTransform = goalNode->transformation;
    pBone = m_goalAnimation->FindBone(nodeName);
    if (pBone)
    {
        pBone->Update(m_goalCurrentTime);
        goalNodeTransform = pBone->GetLocalTransform();
    }

    // Blend two matrices
    // Rotation
    glm::quat baseRotation = glm::quat_cast(baseNodeTransform);
    glm::quat goalRotation = glm::quat_cast(goalNodeTransform);
    glm::quat finalRotation = glm::slerp(baseRotation, goalRotation, m_blendFactor);
    glm::mat4 blendedMatrix = glm::toMat4(finalRotation);
    // Position
    blendedMatrix[3] = (1.0f - m_blendFactor) * baseNodeTransform[3] + goalNodeTransform[3] * m_blendFactor;

    glm::mat4 globalTransformation = parentTransform * blendedMatrix;

    std::map<std::string, sBoneInfo> baseBoneInfoMap = m_baseAnimation->GetBoneIDMap();
    if (baseBoneInfoMap.find(nodeName) != baseBoneInfoMap.end())
    {
        int index = baseBoneInfoMap[nodeName].id;
        glm::mat4 baseOffset = baseBoneInfoMap[nodeName].offset;

        std::map<std::string, sBoneInfo> goalBoneInfoMap = m_goalAnimation->GetBoneIDMap();
        glm::mat4 goalOffset = goalBoneInfoMap[nodeName].offset;

        // Blend two matrices
        // Rotation
        glm::quat baseRotation = glm::quat_cast(baseOffset);
        glm::quat goalRotation = glm::quat_cast(goalOffset);
        glm::quat finalRotation = glm::slerp(baseRotation, goalRotation, m_blendFactor);
        glm::mat4 blendedMatrix = glm::toMat4(finalRotation);
        // Position
        blendedMatrix[3] = (1.0f - m_blendFactor) * baseOffset[3] + goalOffset[3] * m_blendFactor;

        m_vecFinalBoneMatrices[index] = globalTransformation * blendedMatrix;
    }

    int count = glm::min(baseNode->childrenCount, goalNode->childrenCount);

    for (size_t i = 0; i < count; i++)
    {
        m_CalculateBlendedBoneTransform(&baseNode->children[i], &goalNode->children[i], globalTransformation);
    }
}



