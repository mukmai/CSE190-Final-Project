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
}

void PhysicsEngine::updateBody() {
	std::vector<int> completedDeletedList;
	for (int i = deletedEntities.size() - 1; i >= 0; i--) {
		int id = deletedEntities[i];
		auto entity = entityManager->entityMap[id];
		//if (entity->updatedPlayerList.size() > entityManager->playerList.size()) {

		//	completedDeletedList.push_back(i);
		//}
		entityManager->entityMap.erase(id);
		auto search = idBodyMap.find(id);
		if (search == idBodyMap.end())
			continue;
		auto body = btRigidBody::upcast(search->second);
		bodyIdMap.erase(body);
		idBodyMap.erase(id);
		dynamicsWorld->removeRigidBody(body);
		entity->deleteBody();
	}
	deletedEntities.clear();

	//for (int i = 0; i < completedDeletedList.size(); i++) {
	//	deletedEntities.erase(deletedEntities.begin() + completedDeletedList[i]);
	//}
	//completedDeletedList.clear();

	auto updatedList = entityManager->getUpdateList(0);
	for (int i = 0; i < updatedList.size(); i++) {
		int entityID = updatedList[i].id;
		auto search = idBodyMap.find(entityID);
		btRigidBody* body;

		// entity exists in map
		if (search != idBodyMap.end()) {
			body = btRigidBody::upcast(search->second);
		}
		// new entity
		else {
			// create new rigidBody
			auto entity = entityManager->entityMap[entityID];
			if (entity->getState()->isDeleted)
				continue;
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
		if (updatedList[i].type == ENTITY_HAND || updatedList[i].type == ENTITY_BODY || updatedList[i].type == ENTITY_HEAD) {
			auto playerState = entityManager->entityMap[updatedList[i].extraData[PLAYER_ID]]->getState();
			auto globalPlayerTranslation = glm::translate(glm::mat4(1), playerState->pos);
			auto globalPlayerRotation = glm::mat4_cast(playerState->rotation);
			globalPos = globalPlayerTranslation * globalPlayerRotation * glm::vec4(updatedList[i].pos, 1);
			//globalRot = playerState->rotation * updatedList[i].rotation;
		}

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
		playerEntity->updateMovement();
	}

	dynamicsWorld->stepSimulation(1.f / timeStep, 10);

	// collision handling
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++) {
		btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto *objA = contactManifold->getBody0();
		auto *objB = contactManifold->getBody1();
		SBaseEntity* entityA = (SBaseEntity *)(objA->getUserPointer());
		SBaseEntity* entityB = (SBaseEntity *)(objB->getUserPointer());
		if (entityA && entityB) {
			if (entityA->collisionGroup == COL_BULLET && entityB->collisionGroup == COL_WALL) {
				entityA->getState()->isDeleted = true;
				deletedEntities.push_back(entityA->getState()->id);
			}
			else if (entityA->collisionGroup == COL_WALL && entityB->collisionGroup == COL_BULLET) {
				entityB->getState()->isDeleted = true;
				deletedEntities.push_back(entityB->getState()->id);
			}
			float diff = glm::length(entityA->getState()->pos - entityB->getState()->pos);
			float radius = entityA->getState()->scale.x + entityB->getState()->scale.x;
			if (diff <= radius + 0.05f) {

			}
			if (entityA->collisionGroup == COL_BULLET && entityB->collisionGroup == COL_BODY) {
				if (entityA->getState()->extraData[PLAYER_ID] == entityB->getState()->extraData[PLAYER_ID])
					continue;
				entityManager->hitPlayer(entityB->getState()->extraData[PLAYER_ID]);
				entityA->getState()->isDeleted = true;
				deletedEntities.push_back(entityA->getState()->id);
			}
			else if (entityA->collisionGroup == COL_BODY && entityB->collisionGroup == COL_BULLET) {
				if (entityB->getState()->extraData[PLAYER_ID] == entityA->getState()->extraData[PLAYER_ID])
					continue;
				entityManager->hitPlayer(entityA->getState()->extraData[PLAYER_ID]);
				entityB->getState()->isDeleted = true;
				deletedEntities.push_back(entityB->getState()->id);
			}
		}
	}

	

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
	if (entity->collisionGroup == COL_BODY)
		return;
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	entity->getState()->pos = bullet::toGlm(trans.getOrigin());
	entity->getState()->rotation = bullet::toGlm(trans.getRotation());
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