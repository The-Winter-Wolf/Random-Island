#include "cLightMoveMode.h"

#include "../objects/light/cLightManager.h"

#include <iostream>

extern cLightManager* g_pLightManager;

#if LIGHT_CHANGE_MODE

cLightMoveMode::~cLightMoveMode(){}

cLightMoveMode::cLightMoveMode()
{
    attenuation = 1.01f;
    angle = 0.1f;

    moveSpeed = 2.0f;
    firstSpeed = 5.0f;
    secondSpeed = 20.0f;

    numberOfLights = g_pLightManager->GetNumberOfLight();
}

void cLightMoveMode::MoveLight(LightEvents event, float eventValue)
{
    cLight* currentLight = g_pLightManager->GetLight(g_SelectedLight);

    if (event == LIGHT_BOOST && eventValue == 1)
    {
        moveSpeed = secondSpeed;
        return;
    }
    if (event == LIGHT_BOOST && eventValue == 0)
    {
        moveSpeed = firstSpeed;
        return;
    }

    if (event == LIGHT_LEFT)
    {
        currentLight->position.x -= eventValue * moveSpeed;
        return;
    }
    if (event == LIGHT_RIGHT)
    {
        currentLight->position.x += eventValue * moveSpeed;
        return;
    }

    if (event == LIGHT_FORWARD)
    {
        currentLight->position.z += eventValue * moveSpeed;
        return;
    }
    if (event == LIGHT_BACKWARD)
    {
        currentLight->position.z -= eventValue * moveSpeed;
        return;
    }

    if (event == LIGHT_UP)
    {
        currentLight->position.y += eventValue * moveSpeed;
        return;
    }
    if (event == LIGHT_DOWN)
    {
        currentLight->position.y -= eventValue * moveSpeed;
        return;
    }

    if (event == LIGHT_LINEAR_MORE)
    {
        currentLight->atten.y *= 1.01f;     // 1.01f
        return;
    }
    if (event == LIGHT_LINEAR_LESS)
    {
        currentLight->atten.y *= 0.99f;     // 1.01f
        return;
    }

    if (event == LIGHT_QUAD_MORE)
    {
        currentLight->atten.z *= 1.01f;     // 1.01f
        return;
    }
    if (event == LIGHT_QUAD_LESS)
    {
        currentLight->atten.z *= 0.99f;     // 1.01f
        return;
    }

    if (event == LIGHT_OUT_ANGLE_MORE)
    {
        currentLight->param1.z += 0.1f;
        return;
    }
    if (event == LIGHT_OUT_ANGLE_LESS)
    {
        currentLight->param1.z -= 0.1f;
        return;
    }

    if (event == LIGHT_IN_ANGLE_MORE)
    {
        currentLight->param1.y += 0.1f;
        return;
    }
    if (event == LIGHT_IN_ANGLE_LESS)
    {
        currentLight->param1.y -= 0.1f;
        return;
    }

    if (event == LIGHT_ROTATE_FX)
    {
        if (currentLight->direction.x >= 1.0f)
            currentLight->direction.x = 1.0f;

        else currentLight->direction.x += 0.01f;

        return;
    }
    if (event == LIGHT_ROTATE_BX)
    {
        if (currentLight->direction.x <= -1.0f)
            currentLight->direction.x = -1.0f;

        else currentLight->direction.x -= 0.01f;

        return;
    }

    if (event == LIGHT_ROTATE_FY)
    {
        if (currentLight->direction.y >= 1.0f)
            currentLight->direction.y = 1.0f;

        else currentLight->direction.y += 0.01f;

        return;
    }
    if (event == LIGHT_ROTATE_BY)
    {
        if (currentLight->direction.y <= -1.0f)
            currentLight->direction.y = -1.0f;

        else currentLight->direction.y -= 0.01f;

        return;
    }

    if (event == LIGHT_ROTATE_FZ)
    {
        if (currentLight->direction.z >= 1.0f)
            currentLight->direction.z = 1.0f;

        else currentLight->direction.z += 0.01f;

        return;
    }
    if (event == LIGHT_ROTATE_BZ)
    {
        if (currentLight->direction.z <= -1.0f)
            currentLight->direction.z = -1.0f;

        else currentLight->direction.z -= 0.01f;

        return;
    }

    if (event == LIGHT_SELECT)
    {
        if (eventValue < 0)
            g_SelectedLight -= 1;
            
        if (eventValue > 0)
            g_SelectedLight += 1;

        if (g_SelectedLight >= numberOfLights)
            g_SelectedLight = 0;

        if (g_SelectedLight < 0)
            g_SelectedLight = numberOfLights - 1;

        std::cout << "Selected light: " << ::g_SelectedLight << std::endl;
    }
}

#endif // LIGHT_CHANGE_MODE
