#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(SEntityManager * entityManager)
{
	this->entityManager = entityManager;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -5, 0));

	//the ground is a cube of side 100 at position y = -50.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -51.8f, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		// kinematic static objects (hands)
		//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		//body->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	/*
	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}
	*/
}

void PhysicsEngine::updateBody() {
	auto updatedList = entityManager->getUpdateList(0);
	for (int i = 0; i < updatedList.size(); i++) {
		int entityID = updatedList[i].id;
		auto search = idBodyMap.find(entityID);
		btRigidBody* body;

		// entity exists in map
		if (search != idBodyMap.end()) {
			body = search->second;
		}
		// new entity
		else {
			// create new rigidBody
			auto entity = entityManager->entityMap[entityID];
			body = entity->createRigidBody();
			// add into dynamicsWorld
			if (body == nullptr)
				continue;
			dynamicsWorld->addRigidBody(body);
			idBodyMap[entityID] = body;
			bodyIdMap[body] = entityID;
		}
		
		auto transform = body->getWorldTransform();
		btTransform newTransform;
		newTransform.setIdentity();
		newTransform.setOrigin(bullet::fromGlm(updatedList[i].pos));
		//newTransform.setRotation();
		body->setWorldTransform(newTransform);
		body->getMotionState()->setWorldTransform(newTransform);
		body->clearForces();
	}
}

void PhysicsEngine::stepSimulation(float timeStep) {
	updateBody();
	dynamicsWorld->stepSimulation(1.f / timeStep, 10);

	//print positions of all objects
	for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		if (body && body->getMotionState())
		{
			body->getMotionState()->getWorldTransform(trans);
			updateEntity(body);
		}
		else
		{
			trans = obj->getWorldTransform();
		}
		//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
	}
}

void PhysicsEngine::updateEntity(btRigidBody* body) {
	if (bodyIdMap.find(body) == bodyIdMap.end())
		return;

	auto entity = entityManager->entityMap[bodyIdMap[body]];
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	entity->getState()->pos = bullet::toGlm(trans.getOrigin());
	// TODO: update rotation
	entity->updatedPlayerList.clear();
	entity->updatedPlayerList.insert(0);
}

PhysicsEngine::~PhysicsEngine()
{
	//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
}