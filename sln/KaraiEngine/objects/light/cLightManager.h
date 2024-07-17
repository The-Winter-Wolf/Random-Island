#pragma once

#include "../../storage/cConfigData.h"
#include "../../global/common.h"
#include <glm/glm.hpp>

class cLight
{
public:
    cLight();

    glm::vec4 position;
    glm::vec4 diffuse;	    // colour of the light
    glm::vec4 specular;	    // rgb = highlight colour, w = power
    glm::vec4 atten;		// x = constant, y = linear, z = quadratic, w = distanceCutOff
    glm::vec4 direction;	// spot, directional lights
    glm::vec4 param1;	    // x = lightType (0 - point, 1 - spot, 2 - directional)
	                        // y = inner angle, z = outer angle, w = TBD

    glm::vec4 param2;	    // x = 0 for off, 1 for on
};

class cLightManager
{
public:
    ~cLightManager();
    static cLightManager* GetLightManager();

    int GetNumberOfLight();

    void Init();

    void Update();

    cLight* GetLight(unsigned int indexOfLight);

    void TurnOnLight(unsigned int indexOfLight);

    void TurnOffLight(unsigned int indexOfLight);

private:
    cLightManager();
    static cLightManager* m_pLightManager;

    static const unsigned int NUMBER_OF_LIGHTS;

    std::vector<cLight> m_vLights;

    void m_HandLoadLights();

    std::string blockName;
    bool m_InitLightNUB();
    void m_UpdateLightNUB();

    bool m_InitLightUniform();
    void m_UpdateLightUniform();
};

