#pragma once
#include <unordered_map>
#include "CBaseEntity.hpp"

class EntityManager
{
private:
	EntityManager();

	std::unordered_map<int, int> _entityMap;
	std::vector<std::shared_ptr<CBaseEntity>> _entityList;
public:
	// The singleton getter of EntityManager (create one if not exist)
	static EntityManager& getInstance();

	// Get an existing entity using entity id, return nullptr if not exist
	std::shared_ptr<CBaseEntity> getEntity(int id);

	// Get an existing entity using BaseState, create one if not exist
	std::shared_ptr<CBaseEntity> getEntity(std::shared_ptr<BaseState> const& state);

	// Dispatch new state to the entity to update entity
	void update(std::shared_ptr<BaseState> const& state);

	// Render all entities in the game world
	void render(const glm::mat4& projection, const glm::mat4& view);
};

