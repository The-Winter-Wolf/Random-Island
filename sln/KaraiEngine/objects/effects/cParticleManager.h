#pragma once

#include "../object_3D/cObject3D.h"
#include <vector>

class cParticleManager
{
public:
	~cParticleManager();
	static cParticleManager* GetParticleManager();

	struct sParticleInfo
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 velocity = glm::vec3(0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f);
		float lifetime = 0.0f;
		glm::quat orientation = glm::quat(glm::vec3(0.0f));
		glm::quat rotSpeed = glm::quat(glm::vec3(0.0f));
		glm::vec3 scale = glm::vec3(0.0f);
		glm::vec4 color = glm::vec4(1.0f);
	};

	struct sEmitterInfo
	{
		std::string friendlyName;

		unsigned int maxNumParticles = 0;

		glm::vec3 emitterPosition = glm::vec3(0.0f);

		glm::vec3 particlesOffsetMin = glm::vec3(0.0f);
		glm::vec3 particlesOffsetMax = glm::vec3(0.0f);

		glm::vec3 radiansRotationMin = glm::vec3(0.0f);
		glm::vec3 radiansRotationMax = glm::vec3(0.0f);

		glm::vec3 initVelocityMin = glm::vec3(0.0f);
		glm::vec3 initVelocityMax = glm::vec3(0.0f);

		glm::vec4 initColorMin = glm::vec4(1.0f);
		glm::vec4 initColotMax = glm::vec4(1.0f);

		glm::vec3 constantForce = glm::vec3(0.0f);

		unsigned int minParticlesPerTime = 0;
		unsigned int maxParticlesPerTime = 0;

		float lifetimeMin = 0.0f;
		float lifetimeMax = 0.0f;

		bool m_IsEnabled = false;
		bool m_IsVisible = false;
		bool hasColor = false;

		cObject3D* particleObj;

		std::vector<sParticleInfo> m_vecParticles;
	};

	void Init(sEmitterInfo& newEmitter);

	void Explode(std::string emitterName, unsigned int numParticles, float minVelocity, float maxVelocity);

	void Update(float deltaTime);

	cObject3D* GetParticleMesh(std::string emitterName, int numOfParticle);

	std::vector<sEmitterInfo> m_vecEmitters;

private:
	cParticleManager();
	static cParticleManager* m_pParticleManager;

};

