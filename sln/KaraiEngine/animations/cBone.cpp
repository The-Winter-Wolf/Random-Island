#include "cBone.h"

#include "../utils/cAssimpHelper.h"
//#include "../utils/cMaths.h"
//cMaths math;

cBone::cBone(const std::string& name, int ID, const aiNodeAnim* channel)
{
    cAssimpHelper AssimpHelper;

    m_Name = name;
    m_ID = ID;
    m_LocalTransform = glm::mat4(1.0f);

    m_NumPositions = channel->mNumPositionKeys;
    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
    {
        KeyPosition keyPos;
        keyPos.position = AssimpHelper.AssimpToGLM(channel->mPositionKeys[positionIndex].mValue);
        keyPos.timeStamp = channel->mPositionKeys[positionIndex].mTime;
        m_Positions.push_back(keyPos);
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
    {
        KeyRotation keyRot;
        keyRot.orientation = AssimpHelper.AssimpToGLM(channel->mRotationKeys[rotationIndex].mValue);
        keyRot.timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        m_Rotations.push_back(keyRot);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int scaleIndex = 0; scaleIndex < m_NumScalings; ++scaleIndex)
    {
        KeyScale keyScale;
        keyScale.scale = AssimpHelper.AssimpToGLM(channel->mScalingKeys[scaleIndex].mValue);
        keyScale.timeStamp = channel->mScalingKeys[scaleIndex].mTime;        
        m_Scales.push_back(keyScale);
    }
}

cBone::~cBone() {}

void cBone::Update(double animationTime)
{
    glm::mat4 translation = m_InterpolatePosition(animationTime);

    glm::mat4 rotation = m_InterpolateRotation(animationTime);

    glm::mat4 scale = m_InterpolateScaling(animationTime);

    m_LocalTransform = translation * rotation * scale;

    //glm::vec3 vPosition;
    //glm::vec3 vScale;
    //glm::quat vRotation;

    //math.DisassembleMatrix4(m_LocalTransform, vPosition, vScale, vRotation);

    //bool stop = true;
}

glm::mat4 cBone::GetLocalTransform()
{
    return this->m_LocalTransform;
}

std::string cBone::GetBoneName()
{
    return this->m_Name;
}

int cBone::GetBoneID()
{
    return this->m_ID;
}


int cBone::m_GetPositionIndex(double animationTime)
{
    for (int index = 0; index != m_NumPositions; ++index)
    {
        if (animationTime < m_Positions[index + 1].timeStamp)
        {
            return index;
        }
    }

    return -1;
}

int cBone::m_GetRotationIndex(double animationTime)
{
    for (int index = 0; index != m_NumRotations; ++index)
    {
        if (animationTime < m_Rotations[index + 1].timeStamp)
        {
            return index;
        }        
    }
    
    return -1;
}

int cBone::m_GetScaleIndex(double animationTime)
{
    for (int index = 0; index != m_NumScalings; ++index)
    {
        if (animationTime < m_Scales[index + 1].timeStamp)
        {
            return index;
        }
    }

    return -1;
}

float cBone::m_GetScaleFactor(double lastTimeStamp, double nextTimeStamp, double animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;

    return scaleFactor;
}

glm::mat4 cBone::m_InterpolatePosition(double animationTime)
{
    if (m_NumPositions == 1)
    {
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);
    }

    int p0Index = m_GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;

    float scaleFactor = m_GetScaleFactor(m_Positions[p0Index].timeStamp,
                                         m_Positions[p1Index].timeStamp, 
                                         animationTime);

    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position,
                                       m_Positions[p1Index].position,
                                       scaleFactor);

    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 cBone::m_InterpolateRotation(double animationTime)
{
    if (m_NumRotations == 1)
    {
        glm::quat orient = m_Rotations[0].orientation;
        glm::quat rotation = glm::normalize(orient);
        return glm::toMat4(rotation);
    }

    int p0Index = m_GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;

    double p0Time = m_Rotations[p0Index].timeStamp;
    double p1Time = m_Rotations[p1Index].timeStamp;

    float scaleFactor = m_GetScaleFactor(p0Time, p1Time, animationTime);

    glm::quat p0Rot = m_Rotations[p0Index].orientation;
    glm::quat p1Rot = m_Rotations[p1Index].orientation;

    glm::quat finalRotation = glm::slerp(p0Rot, p1Rot, scaleFactor);

    finalRotation = glm::normalize(finalRotation);

    return glm::toMat4(finalRotation);
}

glm::mat4 cBone::m_InterpolateScaling(float animationTime)
{
    if (m_NumScalings == 1)
    {
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);
    }

    int p0Index = m_GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;

    float scaleFactor = m_GetScaleFactor(m_Scales[p0Index].timeStamp,
                                         m_Scales[p1Index].timeStamp,
                                         animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale,
                                    m_Scales[p1Index].scale,
                                    scaleFactor);

    return glm::scale(glm::mat4(1.0f), finalScale);
}
