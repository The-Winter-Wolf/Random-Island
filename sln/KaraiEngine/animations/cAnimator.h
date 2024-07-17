#pragma once

#include "cAnimation.h"
#include "cAnimStates.h"

class cAnimator
{
public:
    cAnimator();
    ~cAnimator();

    void AddAnimation(cAnimation* pAnimation, const eAnimStates& animState);
    void DeleteAnimation(const eAnimStates& animState);

    void PlayAnimation(const eAnimStates& animState, float speed);

    void UpdateAnimation(float dt);

    std::vector<glm::mat4> GetFinalBoneMatrices();

    float GetCurrentTime();
    float GetDuration();

    bool GetFinishedFlag();
    bool GetRunningFlag();

    void SetAnimLock(bool true_false);
    bool GetAnimLock();

    void ResetTime();
    void IsRunning(bool true_false);

    bool GetAnimTrigger(eAnimStates state, float procentTime);

private:
    cAnimStates m_AnimState;
    eAnimStates m_goalAnimState;
    eAnimStates m_baseAnimState;
    eAnimStates m_nextAnimState;

    void m_BlendAnimations(cAnimation* baseAnimation, cAnimation* goalAnimation);
    void m_ChangeAnimation();
    void m_UpdateBoneMatrices();
    void m_CalculateBoneTransform(const sAssimpNodeData* node, glm::mat4 parentTransform);
    void m_CalculateBlendedBoneTransform(const sAssimpNodeData* baseNode, const sAssimpNodeData* goalNode, 
                                         const glm::mat4& parentTransform);

    std::vector<glm::mat4> m_vecFinalBoneMatrices;

    std::vector<glm::mat4> m_vecFutureBoneMatrices;
    std::vector<glm::mat4> m_vecPastBoneMatrices;

    glm::vec3 m_pastPos, m_futurePos;
    glm::vec3 m_pastScale, m_futureScale;
    glm::quat m_pastRot, m_futureRot;

    std::map<eAnimStates /*states*/,cAnimation* /*animation*/> m_mapAnimations;
    std::map<eAnimStates, cAnimation*>::iterator m_animIterator;

    bool bIsAnimChanging;


    cAnimation* m_baseAnimation;
    cAnimation* m_goalAnimation;

    float m_deltaTime;
    float m_speedFactor;

    float m_timer;
    float m_changingTime;

    float m_blendFactor;

    float m_baseCurrentTime;
    float m_goalCurrentTime;

    bool bIsLocked;
    bool bIsFinished;
    bool bIsHit;
    bool bIsRunning;
};

