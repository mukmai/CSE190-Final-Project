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

	generateEnvironment();
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
			body = entity->createRigidBody(dynamicsWorld);
			// add into dynamicsWorld
			if (body == nullptr)
				continue;
			idBodyMap[entityID] = body;
			bodyIdMap[body] = entityID;
		}
		
		auto transform = body->getWorldTransform();

		// if it is body part then need to get global position through body position
		glm::vec3 globalPos;
		glm::quat globalRot = updatedList[i].rotation;
		if (updatedList[i].type == ENTITY_HAND) {
			auto playerState = entityManager->entityMap[updatedList[i].extraData[PLAYER_ID]]->getState();
			auto globalPlayerTranslation = glm::translate(glm::mat4(1), playerState->pos);
			auto globalPlayerRotation = glm::mat4_cast(playerState->rotation);
			globalPos = globalPlayerTranslation * globalPlayerRotation * glm::vec4(updatedList[i].pos, 1);
			//globalRot = playerState->rotation * updatedList[i].rotation;
		}
		//if (updatedList[i].type == ENTITY_PLAYER) {
		//	auto tempEntity = entityManager->entityMap[updatedList[i].id];
		//	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);
		//	auto headState = playerEntity->head->getState();
		//	globalPos = updatedList[i].pos + glm::vec3(1,0,1) * headState->pos;
		//	std::cout << "before: " << tempEntity->getState()->pos.x << " " << tempEntity->getState()->pos.y << " " << tempEntity->getState()->pos.z << std::endl;
		//	//globalRot = playerState->rotation * updatedList[i].rotation;
		//}
		else {
			globalPos = updatedList[i].pos;
		}

		btTransform newTransform;
		newTransform.setIdentity();
		newTransform.setOrigin(bullet::fromGlm(globalPos));
		newTransform.setRotation(bullet::fromGlm(globalRot));
		body->setWorldTransform(newTransform);
		body->getMotionState()->setWorldTransform(newTransform);
		body->clearForces();
	}
}

void PhysicsEngine::stepSimulation(float timeStep) {
	updateBody();
	for (int i = 0; i < entityManager->playerList.size(); i++) {
		auto tempEntity = entityManager->entityMap[entityManager->playerList[i]];
		auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);
		//auto headState = playerEntity->head->getState();
		//btTransform newTransform;
		//newTransform.setIdentity();
		//newTransform.setOrigin(bullet::fromGlm(playerEntity->getState()->pos + glm::vec3(1, 0, 1) * headState->pos));
		////playerEntity->getRigidBody()->setWorldTransform(newTransform);
		////playerEntity->getRigidBody()->getMotionState()->setWorldTransform(newTransform);
		playerEntity->updateMovement();
	}

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
	}
}

void PhysicsEngine::updateEntity(btRigidBody* body) {
	if (bodyIdMap.find(body) == bodyIdMap.end())
		return;

	auto entity = entityManager->entityMap[bodyIdMap[body]];

	// dont update body part
	if (entity->getState()->type == ENTITY_HAND)
		return;
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	entity->getState()->pos = bullet::toGlm(trans.getOrigin());
	//if (entity->getState()->type == ENTITY_PLAYER) {
	//	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(entity);
	//	auto headState = playerEntity->head->getState();
	//	//entity->getState()->pos -= glm::vec3(1, 0, 1) * headState->pos;
	//	std::cout << "after: " << entity->getState()->pos.x << " " << entity->getState()->pos.y << " " << entity->getState()->pos.z << std::endl;
	//}
	entity->getState()->rotation = bullet::toGlm(trans.getRotation());
	entity->updatedPlayerList.clear();
	entity->updatedPlayerList.insert(0);
}

void PhysicsEngine::generateEnvironment() {
	btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(25.), btScalar(25.), btScalar(25.)));

	collisionShapes.push_back(groundShape);

	std::vector<btVector3> wallsPos;
	wallsPos.push_back(btVector3(0, -25.0f, 0)); // bottom
	wallsPos.push_back(btVector3(0, 0, 50.0f)); // front
	wallsPos.push_back(btVector3(0, 0, -50.0f)); // back
	wallsPos.push_back(btVector3(50.0f, 0, 0)); // left
	wallsPos.push_back(btVector3(-50.0f, 0, 0)); // right

	for (int i = 0; i < wallsPos.size(); i++) {
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(wallsPos[i]);

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

		int collideWith = COL_HEIGHT | COL_BULLET | COL_WALL;

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body, COL_WALL, collideWith);
	}
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