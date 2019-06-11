#include "SBaseEntity.hpp"

SBaseEntity::~SBaseEntity()
{
	_state = nullptr;
}

std::shared_ptr<BaseState> SBaseEntity::getState()
{
	return _state;
}

void SBaseEntity::initState(bool generateId)
{
	if (generateId) {
		_state->id = IDGenerator::getInstance()->getNextId();
	}

	_state->pos = glm::vec3(0);
	_state->rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	_state->scale = glm::vec3(1);
}

std::vector<std::shared_ptr<SBaseEntity>> SBaseEntity::getChildren()
{
	return std::vector<std::shared_ptr<SBaseEntity>>();
}

btRigidBody * SBaseEntity::createRigidBody(btDiscreteDynamicsWorld * dynamicsWorld)
{
	rigidBody = nullptr;
	return nullptr;
}

btGhostObject * SBaseEntity::createGhostObject()
{
	return nullptr;
}

btCollisionShape * SBaseEntity::getcollisionShape()
{
	return colShape;
}

btRigidBody * SBaseEntity::getRigidBody()
{
	return rigidBody;
}

void SBaseEntity::deleteBody()
{
	delete colShape;
	delete rigidBody->getMotionState();
	delete rigidBody;
}
