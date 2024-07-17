#include "cCommands.h"

#include "../enemy/cEnemyBuilder.h"
#include "../enemy/iEnemy.h"

#include "../player/cPlayer.h"
extern cPlayer* g_pPlayer;

#include <glm/gtx/vector_angle.hpp>
#include <glm/glm.hpp>

#include "../../global/global.h"
#include <glm/glm.hpp>

cCommands::cCommands()
{
	this->bIsAutoRespawn = true;
}

cCommands::~cCommands()
{
}

void cCommands::HitObject(cObject3D* object, cObject3D* subject)
{
	sGameplay* objGameComp = object->GetGameplayComponent();
	sGameplay* subGameComp = subject->GetGameplayComponent();

	if (subGameComp->bImmortal)
	{
		return;
	}

	subGameComp->Health = subGameComp->Health - objGameComp->Damage;

	if (subGameComp->Health <= 0.0f)
	{
		if (objGameComp->bIsPlayer)
		{
			objGameComp->Exp += subGameComp->Exp;
		}
		
		this->KillObject(subject);

		return;
	}

	if (subGameComp->bIsPlayer)
	{
		std::string name = subject->friendlyName;
		std::string hp = std::to_string(subGameComp->Health);
		std::string mp = std::to_string(subGameComp->Mana);
		std::string st = std::to_string(subGameComp->Stamina);
		std::string exp = std::to_string(subGameComp->Exp);

		g_pConsole->cOutput(name + ": HP[" + hp + "], MP[" + mp + "], ST[" + st + "], EXP[" + exp + "]");
	}
	else
	{
		std::string name = subject->friendlyName;
		std::string hp = std::to_string(subGameComp->Health);

		g_pConsole->cOutput(name + ": HP[" + hp + "]");
	}
}

void cCommands::KillObject(cObject3D* object)
{
	sGameplay* objGameComp = object->GetGameplayComponent();

	if (objGameComp->bImmortal)
	{
		return;
	}

	object->bIsVisible = false;

	objGameComp->bIsDead = true;
	objGameComp->Health = 0.0f;
	objGameComp->Mana = 0.0f;
	objGameComp->Stamina = 0.0f;

	if (objGameComp->bIsPlayer)
	{
		objGameComp->Exp = 0.0f;
	}

	std::string name = object->friendlyName;
	g_pConsole->cOutput("The " +name+ " is dead.");

	this->RespawnObject(object);
}

void cCommands::RespawnObject(cObject3D* object)
{
	sGameplay* objGameComp = object->GetGameplayComponent();

	object->bIsVisible = true;

	objGameComp->bIsDead = false;
	objGameComp->Health = 100.0f;
	objGameComp->Mana = 100.0f;
	objGameComp->Stamina = 100.0f;

	std::string name = object->friendlyName;
	g_pConsole->cOutput("The " + name + " is resurrected.");

	if (objGameComp->bIsPlayer)
	{
		object->SetPosition(WORLD_POS);
	}
	else
	{
		cEnemyBuilder builder;
		glm::vec3 pos = builder.GetRandomPosition(g_pPlayer->GetPlayerObject()->GetPosition(), 40.0f, 15.0f, true);
		object->SetPosition(pos);
	}
}

void cCommands::EarnExp(cObject3D* object, float exp)
{
	object->GetGameplayComponent()->Exp += exp;
}

void cCommands::RestoreHP(cObject3D* object, float hp)
{
	if (object->GetGameplayComponent()->Health + hp >= 100.0f)
	{
		object->GetGameplayComponent()->Health = 100.0f;
	}
	else
	{
		object->GetGameplayComponent()->Health += hp;
	}
}

void cCommands::RestoreMP(cObject3D* object, float mp)
{
	if (object->GetGameplayComponent()->Mana + mp >= 100.0f)
	{
		object->GetGameplayComponent()->Mana = 100.0f;
	}
	else
	{
		object->GetGameplayComponent()->Mana += mp;
	}
}

void cCommands::RestoreST(cObject3D* object, float st)
{
	if (object->GetGameplayComponent()->Stamina + st >= 100.0f)
	{
		object->GetGameplayComponent()->Stamina = 100.0f;
	}
	else
	{
		object->GetGameplayComponent()->Stamina += st;
	}
}

void cCommands::UpdateObjState(const std::vector<iEnemy*>& enemies)
{
	if (g_pPlayer->GetPlayerObject()->GetPosition().y < -2.0f)
	{
		KillObject(g_pPlayer->GetPlayerObject());
	}
	if (g_pPlayer->GetPlayerObject()->GetGameplayComponent()->bIsHit)
	{
		g_pPlayer->GetPlayerObject()->GetGameplayComponent()->bIsHit = false;

		for (int i = 0; i < enemies.size(); i++)
		{
			if (this->m_CheckFieldOfView(g_pPlayer->GetPlayerObject(), enemies[i]->GetEnemyObject(), 3.0f, 10.0f))
			{
				this->HitObject(g_pPlayer->GetPlayerObject(), enemies[i]->GetEnemyObject());
			}
		}
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i]->GetEnemyObject()->GetPosition().y < -2.0f)
		{
			this->KillObject(enemies[i]->GetEnemyObject());
		}
		if (enemies[i]->GetEnemyObject()->GetGameplayComponent()->bIsHit)
		{
			enemies[i]->GetEnemyObject()->GetGameplayComponent()->bIsHit = false;

			if (this->m_CheckFieldOfView(enemies[i]->GetEnemyObject(), g_pPlayer->GetPlayerObject(), 2.5f, 5.0f))
			{
				this->HitObject(enemies[i]->GetEnemyObject(), g_pPlayer->GetPlayerObject());
			}
		}
	}
}

bool cCommands::m_CheckFieldOfView(cObject3D* object, cObject3D* subject, float minDist, float minAngle)
{
	if (glm::distance(object->GetPosition(), subject->GetPosition()) > minDist)
	{
		return false;
	}

	glm::vec3 goalDir = subject->GetPosition() - object->GetPosition();
	goalDir = glm::normalize(goalDir);

	if (glm::angle(goalDir, object->GetDirection()) > minAngle)
	{
		return false;
	}

	return true;
}

