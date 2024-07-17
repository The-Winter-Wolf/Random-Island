#include "cInputManager.h"

#include "../global/global.h"

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../world/player/cPlayer.h"
extern cPlayer* g_pPlayer;

#include "../objects/effects/cParticleManager.h"
extern cParticleManager* g_pParticleManager;

#include "../system/lua/cLuaBrain.h"
extern cLuaBrain g_LuaBrain;

#include "../scenes/cSceneManager.h"
extern cSceneManager* g_pSceneManager;

#include "cRenderManager.h"
extern cRenderManager* g_pRenderManager;

//#include "../storage/cConfigurator.h"
//extern cConfigurator* g_pConfig;

#include <iostream>

#if OBJECT_CHANGE_MODE
#include "../debugs/cObjectMoveMode.h"
cObjectMoveMode* g_pObjMoveManager = new cObjectMoveMode();
#endif // OBJECT_CHANGE_MODE

#if LIGHT_CHANGE_MODE
#include "../debugs/cLightMoveMode.h"
cLightMoveMode* g_pLightMoveManager = new cLightMoveMode();
#endif LIGHT_CHANGE_MODE

cInputManager* cInputManager::m_pInputManager = NULL;

cInputManager* cInputManager::GetInputManager(GLFWwindow* window)
{
    if (cInputManager::m_pInputManager == NULL)
    {
        cInputManager::m_pInputManager = new cInputManager(window);
    }
    return cInputManager::m_pInputManager;
}

cInputManager::cInputManager(GLFWwindow* window)
{
    this->window = window;

    this->tempTab = false;
    this->TAB_MODE = false;

    this->tempCapsLock = false;
    this->CAPS_LOCK_MODE = false;
}

cInputManager::~cInputManager(){}

void cInputManager::KeyCallback(int key, int scancode, int action, int mods)
{
#if OBJECT_CHANGE_MODE
    if (key == 280 /*CAPS_LOCK*/ && action == 1 && CAPS_LOCK_MODE == false)
    {
        tempTab = false;
        tempCapsLock = true;
        g_pObjMoveManager->MoveModel(OBJECT_MOVE, 1);
    }
    if (key == 280 /*CAPS_LOCK*/ && action == 1 && CAPS_LOCK_MODE == true || 
        key == 258 /*TAB*/ && action == 0 && tempTab == true && CAPS_LOCK_MODE == true)
    {
        tempCapsLock = false;
        g_pObjMoveManager->MoveModel(OBJECT_MOVE, 0);
    }
#endif // OBJECT_CHANGE_MODE

#if LIGHT_CHANGE_MODE
    if (key == 258 /*TAB*/ && action == 1 && TAB_MODE == false)
    {
        tempCapsLock = false;
        tempTab = true;
        std::cout << "LIGHT MOVE MODE ON" << std::endl;
    }
    if (key == 258 /*TAB*/ && action == 1 && TAB_MODE == true ||
        key == 280 /*CAPS_LOCK*/ && action == 0 && tempCapsLock == true && TAB_MODE == true)
    {
        tempTab = false;
        std::cout << "LIGHT MOVE MODE OFF" << std::endl;
    }
#endif // LIGHT_CHANGE_MODE

    if (key == 280 /*CAPS_LOCK*/ && action == 0 || key == 258 /*TAB*/ && action == 0)
    {
        CAPS_LOCK_MODE = tempCapsLock;
        TAB_MODE = tempTab;
    }
    
    if (key == 96 /*~*/ && action == 0)
    {
        //if (g_bDrawDebugObjects)
        //{
        //    g_bDrawDebugObjects = false;
        //}
        //else
        //{
        //    g_bDrawDebugObjects = true;
        //}
    }

    if (key == 262 /*RIGHT*/ && action == 0)
    {

    }
    if (key == 263 /*LEFT*/ && action == 0)
    {

    }
    if (key == 265 /*UP*/ && action == 0)
    {

    }
    if (key == 264 /*DOWN*/ && action == 0)
    {

    }

    if (key == 49 /*1*/ && action == 0)
    {
        g_pSceneManager->Input(1);
    }
    if (key == 50 /*2*/ && action == 0)
    {
        g_pSceneManager->Input(2);
    }
    if (key == 51 /*3*/ && action == 0)
    {
        g_pSceneManager->Input(3);
    }
    if (key == 52 /*4*/ && action == 0)
    {
        g_pSceneManager->Input(4);
    }
    if (key == 53 /*5*/ && action == 0)
    {
        g_pSceneManager->Input(5);
    }
    if (key == 54 /*6*/ && action == 0)
    {
        g_pSceneManager->Input(6);
    }
    if (key == 55 /*7*/ && action == 0)
    {
        g_pSceneManager->Input(7);
    }
    if (key == 56 /*8*/ && action == 0)
    {
        g_pSceneManager->Input(8);
    }
    if (key == 57 /*9*/ && action == 0)
    {
        g_pSceneManager->Input(9);
    }
    if (key == 48 /*0*/ && action == 0)
    {
        g_pSceneManager->Input(10);
    }
    if (key == 45 /*-*/ && action == 0)
    {
        g_pSceneManager->Input(11);
    }
    if (key == 61 /*+*/ && action == 0)
    {
        g_pSceneManager->Input(12);
    }

    return;
}

void cInputManager::MouseCallback(int button, int action, int mods)
{
    if (g_pCamera->GetCameraMod() == THIRD_PERSON && g_pPlayer)
    {
        if (!bIsMouseLock)
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                g_pPlayer->MovePlayer(PLAYER_LEFT_ATTACK, action);
            }

            if (button == GLFW_MOUSE_BUTTON_RIGHT)
            {
                g_pPlayer->MovePlayer(PLAYER_RIGHT_ATTACK, action);
            }
        }
    }
}

void cInputManager::ScrollCallback(double xoffset, double yoffset)
{
    if ( ! CAPS_LOCK_MODE)
    {
        if (!TAB_MODE)
        {
            /*g_pCamera->ScrollAction(yoffset);*/
        }

#if LIGHT_CHANGE_MODE
        else
        {
            g_pLightMoveManager->MoveLight(LIGHT_SELECT, yoffset);
        }
#endif // LIGHT_CHANGE_MODE
    }

#if OBJECT_CHANGE_MODE
    else 
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        {
            g_pObjMoveManager->MoveModel(OBJECT_SCALE, yoffset);
        }
        else g_pObjMoveManager->MoveModel(OBJECT_SELECT, yoffset);
    }
#endif // OBJECT_CHANGE_MODE
}

void cInputManager::KeyboardInput(float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
    { 
        //if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            //g_pConfig->ReadConfigFromFile();
        //if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            //g_pConfig->WriteConfigToFile();
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
        g_LuaBrain.EnterScript();
    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        g_LuaBrain.Update();

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        g_pParticleManager->Explode("Emitter01", 10, 35.0f, 50.0f);

    if ( ! CAPS_LOCK_MODE )
    {
        if ( ! TAB_MODE)
        {
            if (g_pCamera->GetCameraMod() == DYNAMIC)
            {
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_BOOST, 1);
                else g_pCamera->MoveCamera(CAMERA_BOOST, 0);

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_FORWARD, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_BACKWARD, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_LEFT, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_RIGHT, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_SPACE) || 
                    glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_UP, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) || 
                    glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                    g_pCamera->MoveCamera(CAMERA_DOWN, deltaTime);

            }

            if (g_pCamera->GetCameraMod() == THIRD_PERSON && g_pPlayer)
            {
                if (!bIsMoveLock)
                {
                    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                        g_pPlayer->MovePlayer(PLAYER_BOOST, 1);
                    else g_pPlayer->MovePlayer(PLAYER_BOOST, 0);

                    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                        g_pPlayer->MovePlayer(PLAYER_FORWARD, deltaTime);
                    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                        g_pPlayer->MovePlayer(PLAYER_BACKWARD, deltaTime);
                    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                        g_pPlayer->MovePlayer(PLAYER_LEFT, deltaTime);
                    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                        g_pPlayer->MovePlayer(PLAYER_RIGHT, deltaTime);
                    //if (glfwGetKey(window, GLFW_KEY_SPACE) ||
                    //    glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                    //    g_pPlayer->MovePlayer(PLAYER_JUMP, deltaTime);
                    //if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) ||
                    //    glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                    //    g_pPlayer->MovePlayer(PLAYER_SQUAT, deltaTime);
                }
            }
            if (g_pCamera->GetCameraMod() == STATIC)
            {

            }
        }

#if LIGHT_CHANGE_MODE
        else 
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_FX, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_BX, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_FY, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_BY, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_FZ, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_ROTATE_BZ, deltaTime);
            }
            else
            {
                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_BOOST, 1);
                if (!glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_BOOST, 0);

                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_FORWARD, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_BACKWARD, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_LEFT, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_RIGHT, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_UP, deltaTime);
                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                    g_pLightMoveManager->MoveLight(LIGHT_DOWN, deltaTime);
            }

            // Linear attenuation 
            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_LINEAR_LESS, 0);
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_LINEAR_MORE, 0);

            // Quadratic attenuation 
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_QUAD_LESS, 0);
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_QUAD_MORE, 0);

            // Outter angle
            if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_OUT_ANGLE_LESS, 0);
            if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_OUT_ANGLE_MORE, 0);

            // Inner angle
            if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_IN_ANGLE_LESS, 0);
            if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
                g_pLightMoveManager->MoveLight(LIGHT_IN_ANGLE_MORE, 0);

            // Turn on/off debug spheres
            if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
                ::g_bDrawDebugLightSpheres = true;
            if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
                ::g_bDrawDebugLightSpheres = false;
        }
#endif // LIGHT_CHANGE_MODE
    }

#if OBJECT_CHANGE_MODE
    else
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_FX, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_BX, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_FY, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_BY, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_FZ, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_ROTATE_BZ, deltaTime);
        }
        else
        {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_BOOST, 1);
            if (! glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_BOOST, 0);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_UP, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
                g_pObjMoveManager->MoveModel(OBJECT_DOWN, deltaTime);
        } 
    }
#endif // OBJECT_CHANGE_MODE

}
