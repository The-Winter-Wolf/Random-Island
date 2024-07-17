#include "cTime.h"

#include <ctime>

const float cTime::MAX_DELTA_TIME_DEFAULT = 1.0f / 120.0f;

cTime::cTime(unsigned int numberOfSamples)
{
    this->m_LastTime = glfwGetTime();

    this->m_MaxDeltaTime = MAX_DELTA_TIME_DEFAULT;

    for (int count = 0; count != numberOfSamples; count++)
    {
        this->m_vecDeltaTimes.push_back(cTime::MAX_DELTA_TIME_DEFAULT);
    }
}

void cTime::SetMinFPS(float minFPS)
{
    this->m_MaxDeltaTime = 1 / minFPS;
    return;
}

float cTime::GetDeltaTime()
{
    // Get the delta since last time we checked
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_LastTime;

    if (deltaTime > m_MaxDeltaTime)
    {
        deltaTime = m_MaxDeltaTime;
    }

    this->m_LastTime = currentTime;

    this->m_vecDeltaTimes[this->m_NextBufferIndex] = deltaTime;
    this->m_NextBufferIndex++;

    if ( this->m_NextBufferIndex >= static_cast<unsigned int>(this->m_vecDeltaTimes.size()) )
    {
        this->m_NextBufferIndex = 0;
    }

    // Get the average deltaTime
    float totalDeltaTime = 0.0;
    for (unsigned int index = 0; index != this->m_vecDeltaTimes.size(); index++)
    {
        totalDeltaTime += this->m_vecDeltaTimes[index];
    }

    float averageDelta = totalDeltaTime / static_cast<float>( this->m_vecDeltaTimes.size() );

    return averageDelta;
}

float cTime::GetGLFWTime()
{
    return static_cast<float>(glfwGetTime());
}

float cTime::GetFPS()
{
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - m_LastTime;
    return 1 / deltaTime;
    //float averageFPS = 1 / GetDeltaTime();
    //return averageFPS;
}

std::string cTime::GetDateAndTime()
{
    //"2012/10/26 07:15:00"
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    std::string year = std::to_string(now->tm_year + 1900);
    std::string month = std::to_string(now->tm_mon + 1);
    std::string day = std::to_string(now->tm_mday);
    std::string hour = std::to_string(now->tm_hour);
    std::string min = std::to_string(now->tm_min);
    std::string sec = std::to_string(now->tm_sec);

    std::string time = year + '/' + month + '/' + day + ' ' + hour + ':' + min + ':' + sec;

    return time;
}