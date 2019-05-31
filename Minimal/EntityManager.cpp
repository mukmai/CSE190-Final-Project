#include "EntityManager.h"
#include <algorithm>
#include <iostream>


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

std::shared_ptr<CBaseEntity> EntityManager::getEntity(std::shared_ptr<BaseState> const & state)
{
	// Check if the Entity is already created
	int id = state->id;
	auto result = _entityMap.find(id);

	// Return if found
	if (result != _entityMap.end())
	{
		return _entityList[result->second];
	}

	// Otherwise create the Entity based on its type
	std::shared_ptr<CBaseEntity> entity = nullptr;
	EntityType type = state->type;

	switch (type)
	{
	case ENTITY_HAND:
		break;
	}

	if (entity)
	{
		_entityList.push_back(entity);
		_entityMap.insert({ id, _entityList.size() - 1 });
	}

	return entity;
}

void EntityManager::update(std::shared_ptr<BaseState> const & state)
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

void EntityManager::render(const glm::mat4 & projection, const glm::mat4 & view)
{
	for (int i = 0; i < _entityList.size(); i++) {
		if (_entityList[i]) {
			//_entityList[i]->render(projection, view);
			auto state = _entityList[i]->getState();
			std::cout << "Player Position: " << state->id << std::endl;
		}
	}
}