#pragma once
#include "physx/PxPhysicsAPI.h"
#include "SEntityManager.h"

class PhysicsManager
{
public:
	PhysicsManager(SEntityManager* entityManager);
	~PhysicsManager();

private:
	SEntityManager * entityManager;
};

