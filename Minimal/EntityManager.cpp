#include "EntityManager.h"
#include <algorithm>
#include <iostream>
#include "CHandEntity.hpp"
#include "CPlayerEntity.hpp"
#include "CHeadEntity.hpp"
#include "CBodyEntity.hpp"
#include "CSphereEntity.hpp"
#include "CBoxEntity.hpp"

EntityManager::EntityManager()
{
}

EntityManager & EntityManager::getInstance()
{
	static EntityManager entityManager;
	return entityManager;
}

std::shared_ptr<CBaseEntity> EntityManager::getEntity(int id)
{
	auto result = _entityMap.find(id);

	// Return if found
	if (result != _entityMap.end())
	{
		return _entityList[result->second];
	}
	return nullptr;
}

std::shared_ptr<CBaseEntity> EntityManager::getEntity(BaseState const & state)
{
	// Check if the Entity is already created
	int id = state.id;
	auto result = _entityMap.find(id);

	// Return if found
	if (result != _entityMap.end())
	{
		return _entityList[result->second];
	}

	// Otherwise create the Entity based on its type
	std::shared_ptr<CBaseEntity> entity = nullptr;
	EntityType type = state.type;

	switch (type)
	{
	case ENTITY_HAND:
		entity = std::make_shared<CHandEntity>(state.extraData[HAND_INDEX]);
		break;
	case ENTITY_PLAYER:
		entity = std::make_shared<CPlayerEntity>();
		break;
	case ENTITY_HEAD:
		entity = std::make_shared<CHeadEntity>();
		break;
	case ENTITY_BODY:
		entity = std::make_shared<CBodyEntity>();
		break;
	case ENTITY_SPHERE:
		entity = std::make_shared<CSphereEntity>();
		break;
	case ENTITY_BOX:
		entity = std::make_shared<CBoxEntity>();
		break;
	}

	if (entity)
	{
		_entityList.push_back(entity);
		_entityMap.insert({ id, _entityList.size() - 1 });
	}

	return entity;
}

void EntityManager::update(BaseState const & state)
{
	auto entity = getEntity(state);

	if (entity)
	{
		entity->updateState(state);
	}

	// Destroy entity if necessary
	//if (state->isDestroyed)
	//{
	//	// Find in map and destroy if it exists
	//	auto result = _entityMap.find(state->id);
	//	if (result != _entityMap.end())
	//	{
	//		//_entityList.erase(_entityList.begin() + result->second);

	//		// Erase from map
	//		_entityList[result->second] = nullptr;
	//		_entityMap.erase(result);
	//	}

	//	return;
	//}
}

void EntityManager::render(const glm::mat4 & projection, const glm::mat4 & view, glm::vec3 eyePos)
{
	//std::cout << "Entities ID: ";
	for (int i = 0; i < _entityList.size(); i++) {
		if (_entityList[i]) {
			_entityList[i]->render(projection, view, eyePos);
			auto state = _entityList[i]->getState();
			//std::cout << state->id << " ";
		}
	}
	//std::cout << std::endl;
}

void EntityManager::setPlayerID(int playerID)
{
	this->playerID = playerID;
}

int EntityManager::getPlayerID()
{
	return playerID;
}
