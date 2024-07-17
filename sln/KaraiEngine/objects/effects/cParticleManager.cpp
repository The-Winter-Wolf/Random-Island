#include "cParticleManager.h"

#include "../../utils/cRandomUtils.h"

extern cRandomUtils* g_pRandUtils;

cParticleManager* cParticleManager::m_pParticleManager = NULL;

cParticleManager* cParticleManager::GetParticleManager()
{
	if (cParticleManager::m_pParticleManager == NULL)
	{
		cParticleManager::m_pParticleManager = new cParticleManager();
	}
	return cParticleManager::m_pParticleManager;
}

cParticleManager::cParticleManager(){}

cParticleManager::~cParticleManager(){}

void cParticleManager::Init(sEmitterInfo& newEmitter)
{
	for (std::vector<sEmitterInfo>::iterator it = m_vecEmitters.begin(); it != m_vecEmitters.end(); it++)
	{
		if (it->friendlyName == newEmitter.friendlyName)
		{
			this->m_vecEmitters.erase(it);
			break;
		}
	}

	sEmitterInfo newEmitterInfo = newEmitter;
	newEmitterInfo.m_vecParticles.reserve(newEmitterInfo.maxNumParticles);

	for (unsigned int count = 0; count != newEmitterInfo.maxNumParticles; count++)
	{
		sParticleInfo newParticle;		
		newEmitterInfo.m_vecParticles.push_back(newParticle);
	}

	this->m_vecEmitters.push_back(newEmitterInfo);

	return;
}

void cParticleManager::Update(float deltaTime)
{
	for (sEmitterInfo &emitter : m_vecEmitters)
	{
		if (!emitter.m_IsEnabled)
		{
			return;
		}

		unsigned int numParticlesPerTime = g_pRandUtils->GetRandomInt(emitter.minParticlesPerTime, emitter.maxParticlesPerTime);

		unsigned int particlesCreatedPerTime = 0;

		for (sParticleInfo& particle : emitter.m_vecParticles)
		{
			if (particle.lifetime <= 0.0)
			{
				if (particlesCreatedPerTime >= numParticlesPerTime)
				{
					continue;
				}

				particle.lifetime = g_pRandUtils->GetRandomFloat(emitter.lifetimeMin, emitter.lifetimeMax);

				particle.position = emitter.emitterPosition;
				particle.position += g_pRandUtils->GetRandomVec3(emitter.particlesOffsetMin, emitter.particlesOffsetMax);

				particle.velocity = g_pRandUtils->GetRandomVec3(emitter.initVelocityMin, emitter.initVelocityMax);

				particle.rotSpeed = glm::quat(g_pRandUtils->GetRandomVec3(emitter.radiansRotationMin, emitter.radiansRotationMax));

				particle.acceleration = emitter.constantForce;

				particle.color = g_pRandUtils->GetRandomVec4(emitter.initColorMin, emitter.initColotMax);

				particlesCreatedPerTime++;
			}

			if (particle.lifetime > 0.0)
			{
				particle.velocity += particle.acceleration * deltaTime;

				particle.position += particle.velocity * deltaTime;

				particle.orientation *= particle.rotSpeed;

				particle.lifetime -= deltaTime;
			}
		}
	}

	return;
}

cObject3D* cParticleManager::GetParticleMesh(std::string emitterName, int numOfParticle)
{
	for (sEmitterInfo &emitter : m_vecEmitters)
	{
		if (emitter.friendlyName == emitterName)
		{
			if (numOfParticle >= emitter.m_vecParticles.size())
			{
				break;
			}

			sParticleInfo particle = emitter.m_vecParticles[numOfParticle];

			if (particle.lifetime <= 0.0f)
			{
				break;
			}

			emitter.particleObj->bHasColor = emitter.hasColor;
			emitter.particleObj->SetColor(particle.color);
			emitter.particleObj->SetPosition(particle.position);
			emitter.particleObj->SetQuatOrientation(particle.orientation);

			return emitter.particleObj;
		}
	}

	return nullptr;
}

void cParticleManager::Explode(std::string emitterName, unsigned int numParticles, float minVelocity, float maxVelocity)
{
	for (sEmitterInfo& emitter : m_vecEmitters)
	{
		if (emitter.friendlyName != emitterName)
		{
			continue;
		}

		unsigned int particlesCreatedPerTime = 0;

		for (sParticleInfo& particle : emitter.m_vecParticles)
		{
			if (particlesCreatedPerTime >= numParticles)
			{
				break;
			}

			if (particle.lifetime <= 0.0)
			{
				particle.lifetime = g_pRandUtils->GetRandomFloat(emitter.lifetimeMin, emitter.lifetimeMax);

				particle.position = emitter.emitterPosition;

				glm::vec3 randDirection = g_pRandUtils->GetRandomVec3(glm::vec3(-1.0f), glm::vec3(1.0f));

				particle.velocity = randDirection * g_pRandUtils->GetRandomFloat(minVelocity, maxVelocity);

				particle.rotSpeed = glm::quat(g_pRandUtils->GetRandomVec3(emitter.radiansRotationMin, emitter.radiansRotationMax));

				particle.acceleration = emitter.constantForce;

				particle.color = g_pRandUtils->GetRandomVec4(emitter.initColorMin, emitter.initColotMax);

				particlesCreatedPerTime++;
			}
		}
	}

	return;
}


