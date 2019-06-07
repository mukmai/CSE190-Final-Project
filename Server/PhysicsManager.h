#pragma once
#include "physx/PxPhysicsAPI.h"
#include "SEntityManager.h"


using namespace physx;

#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")

class PhysicsManager
{
public:
	PhysicsManager(SEntityManager* entityManager);
	~PhysicsManager();

private:
	SEntityManager * entityManager;

	void InitializePhysX();


};

