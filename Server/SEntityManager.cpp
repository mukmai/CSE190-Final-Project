#include "SEntityManager.h"
#include "SHandEntity.hpp"

SEntityManager::SEntityManager()
{
}

std::vector<BaseState> SEntityManager::getUpdateList(int playerID)
{
	std::vector<BaseState> res;
	
	// Loop through all entities and insert state to res if entity is not updated to player
	for (auto& entityPair : entityMap) {
		auto curEntity = entityPair.second;
		if (curEntity->updatedPlayerList.find(playerID) == curEntity->updatedPlayerList.end()) {
			curEntity->updatedPlayerList.insert(playerID);
			res.push_back(*(curEntity->getState()));
		}
	}

	return res;
}

void SEntityManager::addPlayerEntity(int playerID)
{
	auto leftHandEntity = std::make_shared<SHandEntity>();
	entityMap.insert({ leftHandEntity->getState()->id, leftHandEntity });
}
