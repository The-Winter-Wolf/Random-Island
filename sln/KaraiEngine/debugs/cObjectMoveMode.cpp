#include "cObjectMoveMode.h"

#include "../objects/camera/cCamera.h"

#include <iostream>

#if OBJECT_CHANGE_MODE

cObjectMoveMode::cObjectMoveMode()
{
    bHasLight = false;
    bHasColor = false;
    colorOfModel = glm::vec4(0);

    moveSpeed = 2.0f;
    firstSpeed = 5.0f;
    secondSpeed = 20.0f;

    m_scale = 1.1f;

    mass = 0.0f;
    lastPos = glm::vec3(0.0f);

    m_pObject = nullptr;
}

cObjectMoveMode::~cObjectMoveMode(){}

void cObjectMoveMode::MoveModel(ObjectEvents event, float eventValue)
{
    if (event == OBJECT_BOOST && eventValue == 1)
    {
        moveSpeed = secondSpeed;
        return;
    }
    if (event == OBJECT_BOOST && eventValue == 0)
    {
        moveSpeed = firstSpeed;
        return;
    }

    if (event == OBJECT_MOVE && eventValue == 1)
    {   
        m_pObject = g_vpObjectToDraw[::g_SelectedObject];
        //m_pObject = g_pPhysicsEngine->vpPhysObjects[0]->pAssocMesh;
        //m_pPhysObject = g_pPhysicsEngine->vpPhysObjects[0];

        //mass = pObject->GetMass();
        bHasLight = m_pObject->bHasLight;
        bHasColor = m_pObject->bHasColor;
        colorOfModel = m_pObject->GetColor();

        //pObject->SetMass(0.0f);
        m_pObject->bHasLight = false;
        m_pObject->bHasColor = true;
        m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.1f));

        std::cout << "OBJECT MOVE MODE ON" << std::endl;
        return;
    }
    if (event == OBJECT_MOVE && eventValue == 0)
    {
        //pObject->SetMass(mass);
        m_pObject->bHasLight = bHasLight;
        m_pObject->bHasColor = bHasColor;
        m_pObject->SetColor(colorOfModel);

        std::cout << "OBJECT MOVE MODE OFF" << std::endl;
        return;
    }

    if (event == OBJECT_SELECT)
    {
        //pObject->SetMass(mass);
        m_pObject->bHasLight = bHasLight;
        m_pObject->bHasColor = bHasColor;
        m_pObject->SetColor(colorOfModel);

        if (eventValue < 0) 
            //g_selectedObject -= 1;
            ::g_SelectedObject -= 1;

        if (eventValue > 0)
            //g_selectedObject += 1;
            ::g_SelectedObject += 1;


        int size = g_vpObjectToDraw.size();
        if (::g_SelectedObject >= size)
            ::g_SelectedObject = 0;

        if (::g_SelectedObject < 0)
            ::g_SelectedObject = g_vpObjectToDraw.size() - 1;

        m_pObject = g_vpObjectToDraw[::g_SelectedObject];
        bHasLight = m_pObject->bHasLight;
        bHasColor = m_pObject->bHasColor;
        colorOfModel = m_pObject->GetColor();

        m_pObject->bHasLight = false;
        m_pObject->bHasColor = true;
        m_pObject->SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        //int size = g_pPhysicsEngine->vpPhysObjects.size();
        //if (g_selectedObject >= size)
        //    g_selectedObject = 0;
        //    
        //if (g_selectedObject < 0)
        //    g_selectedObject = g_pPhysicsEngine->vpPhysObjects.size() - 1;

        //pObject = g_pPhysicsEngine->vpPhysObjects[g_selectedObject];
        //m_pObject = g_pPhysicsEngine->vpPhysObjects[g_selectedObject]->pAssocMesh;

        //mass = pObject->GetMass();

        //pObject->SetMass(0.0f);

        return;
    }
    if (event == OBJECT_SCALE)
    {
        if (eventValue < 0)
        {
            glm::vec3 scale = m_pObject->GetScale();
            scale *= abs(eventValue * 1 / m_scale);
            m_pObject->SetScale(scale);
        }

        if (eventValue > 0)
        {
            glm::vec3 scale = m_pObject->GetScale();
            scale *= abs(eventValue * m_scale);
            m_pObject->SetScale(scale);
        }
     
        return;
    }

    if (event == OBJECT_LEFT)
    {
        //lastPos.x = pObject->position.x;
        //pObject->position.x += moveSpeed * eventValue;
        //pObject->velocity.x = (pObject->position.x - lastPos.x) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.x -= moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }
    if (event == OBJECT_RIGHT)
    {
        //lastPos.x = pObject->position.x;
        //pObject->position.x -= moveSpeed * eventValue;
        //pObject->velocity.x = (pObject->position.x - lastPos.x) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.x += moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }

    if (event == OBJECT_FORWARD)
    {
        //lastPos.z = pObject->position.z;
        //pObject->position.z += moveSpeed * eventValue;
        //pObject->velocity.z = (pObject->position.z - lastPos.z) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.z -= moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }
    if (event == OBJECT_BACKWARD)
    {
        //lastPos.z = pObject->position.z;
        //pObject->position.z -= moveSpeed * eventValue;
        //pObject->velocity.z = (pObject->position.z - lastPos.z) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.z += moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }

    if (event == OBJECT_UP)
    {   
        //lastPos.y = pObject->position.y;
        //pObject->position.y -= moveSpeed * eventValue;
        //pObject->velocity.y = (pObject->position.y - lastPos.y) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.y += moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }
    if (event == OBJECT_DOWN)
    {
        //lastPos.y = pObject->position.y;
        //pObject->position.y += moveSpeed * eventValue;
        //pObject->velocity.y = (pObject->position.y - lastPos.y) / eventValue;
        glm::vec3 pos = m_pObject->GetPosition();
        pos.y -= moveSpeed * eventValue;
        m_pObject->SetPosition(pos);
        return;
    }

    if (event == OBJECT_ROTATE_FX)
    {
        //lastPos.x = pObject->orientation.x;
        //pObject->orientation.x -= moveSpeed * eventValue;
        //pObject->velocity.x = (pObject->orientation.x - lastPos.x) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.x += moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }
    if (event == OBJECT_ROTATE_BX)
    {
        //lastPos.x = pObject->orientation.x;
        //pObject->orientation.x += moveSpeed * eventValue;
        //pObject->velocity.x = (pObject->orientation.x - lastPos.x) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.x -= moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }

    if (event == OBJECT_ROTATE_FY)
    {
        //lastPos.y = pObject->orientation.y;
        //pObject->orientation.y -= moveSpeed * eventValue;
        //pObject->velocity.y = (pObject->orientation.y - lastPos.y) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.y += moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }
    if (event == OBJECT_ROTATE_BY)
    {
        //lastPos.y = pObject->orientation.y;
        //pObject->orientation.y += moveSpeed * eventValue;
        //pObject->velocity.y = (pObject->orientation.y - lastPos.y) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.y -= moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }

    if (event == OBJECT_ROTATE_FZ)
    {
        //lastPos.z = pObject->orientation.z;
        //pObject->orientation.z -= moveSpeed * eventValue;
        //pObject->velocity.z = (pObject->orientation.z - lastPos.z) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.z += moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }
    if (event == OBJECT_ROTATE_BZ)
    {
        //lastPos.z = pObject->orientation.z;
        //pObject->orientation.z += moveSpeed * eventValue;
        //pObject->velocity.z = (pObject->orientation.z - lastPos.z) / eventValue;
        glm::vec3 rot = glm::vec3(0);
        rot.z -= moveSpeed * eventValue;
        m_pObject->AdjustAngleFromEuler(rot);
        return;
    }
}

#endif // OBJECT_CHANGE_MODE
