#pragma once
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include <map>
#include <vector>
#include "SEntityManager.h"
#include "SPlayerEntity.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PhysicsEngine
{
public:
	PhysicsEngine(SEntityManager * entityManager);

	void stepSimulation(float timeStep);

	void updateBody();

	void updateEntity(btRigidBody* body);

	void generateEnvironment();

	~PhysicsEngine();

private:
	///collision configuration contains default setup for memory, collision setup.
	btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> objectsCollisions;

	std::map<btRigidBody*, int> bodyIdMap;
	std::map<int, btRigidBody*> idBodyMap;

	SEntityManager * entityManager;
};

