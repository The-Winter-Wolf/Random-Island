#include "cLoadManager.h"

#include "../objects/camera/cCamera.h"
extern cCamera* g_pCamera;

#include "../graphics/models/cModelManager.h"
extern cModelManager* g_pModelManager;

#include "../objects/effects/cParticleManager.h"
extern cParticleManager* g_pParticleManager;

#include "../graphics/textures/cTextureManager.h"
extern cTextureManager* g_pTextureManager;

#include "../physics/cPhysicsManager.h"
extern cPhysicsManager* g_pPhysicsManager;

#include "../animations/cAnimator.h"
extern cAnimator* g_pAnimator1;
extern cAnimator* g_pAnimator2;

#include "../behaviour/cBehaviour.h"
extern cBehaviour* g_pBehaviour;

#include "../global/global.h"

extern cObject3D* g_pDebugSphereObject;

extern std::vector< cObject3D* > g_vpObjectToDraw;

cLoadManager* cLoadManager::m_pLoadManager = NULL;

cLoadManager* cLoadManager::GetLoadManager()
{
    if (cLoadManager::m_pLoadManager == NULL)
    {
        cLoadManager::m_pLoadManager = new cLoadManager();
    }
    return cLoadManager::m_pLoadManager;
}

cLoadManager::cLoadManager()
{
    shaderID = g_ShaderID;
}

cLoadManager::~cLoadManager(){}

void cLoadManager::SetShaderId(int shadID)
{
    shaderID = shadID;
}

void cLoadManager::LoadModels(int numLoad)
{
    if (numLoad == 1 || numLoad == 0)
    {
        g_pModelManager->SetBasePath("../../extern/assets/models");

        g_pModelManager->LoadModelIntoVAO("Flat_1x1_xyz_n_rgba_center.ply", shaderID);
        g_pModelManager->LoadModelIntoVAO("Flat_1x1_xyz_n_rgba_corner.ply", shaderID);
        g_pModelManager->LoadModelIntoVAO("Sphere_r1_xyz_n_rgba.ply", shaderID);
        g_pModelManager->LoadModelIntoVAO("Cube_1x1x1_xyz_n_rgba.ply", shaderID);
    }
}

void cLoadManager::InitModels(int numInit)
{
    // Debug object
    g_pDebugSphereObject = new cObject3D();
    g_pDebugSphereObject->BindModelToObject("Sphere_r1_xyz_n_rgba.ply");
    g_pDebugSphereObject->friendlyName = "DEBUG_SPHERE";
    g_pDebugSphereObject->bIsWireframe = true;
    g_pDebugSphereObject->bHasLight = false;
    g_pDebugSphereObject->SetScale(glm::vec3(1.0f));
    g_pDebugSphereObject->bIsVisible = false;
    g_pDebugSphereObject->bHasColor = true;

    if (numInit == 1 || numInit == 0)
    {
        //cObject3D* pSphereParticle = new cObject3D();
        //pSphereParticle->modelName = "Sphere_r1_xyz_n_rgba.ply";
        //pSphereParticle->friendlyName = "Particle";
        //pSphereParticle->bHasLight = false;
        //pSphereParticle->bHasDebugColor = true;
        //pSphereParticle->SetDebugColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //cParticleManager::sEmitterInfo emitter;
        //emitter.friendlyName = "Emitter01";
        //emitter.particleMesh = pSphereParticle;
        //emitter.emitterPosition = glm::vec3(0.0f, 5.0f, 0.0f);
        //emitter.initVelocityMin = glm::vec3(-3.0f, 10.0f, -3.0f);
        //emitter.initVelocityMax = glm::vec3(3.0f, 15.0f, 3.0f);
        //emitter.radiansRotationMin = glm::vec3(-0.1f, -0.1f, -0.1f);
        //emitter.radiansRotationMin = glm::vec3(0.1f, 0.1f, 0.1f);
        //emitter.initColorMin = glm::vec4(0.0f);
        //emitter.initColotMax = glm::vec4(1.0f);
        //emitter.constantForce = glm::vec3(0.0f, -4.0f, 0.0f);
        //emitter.lifetimeMin = 5.0f;
        //emitter.lifetimeMax = 10.0f;
        //emitter.minParticlesPerTime = 1;
        //emitter.maxParticlesPerTime = 3;

        //emitter.maxNumParticles = 200;
        //emitter.m_IsVisible = true;
        //emitter.m_IsEnabled = true;
        //emitter.hasColor = true;

        //g_pParticleManager->Init(emitter);
    }

    if (numInit == 2 || numInit == 0)
    {

    }
}
