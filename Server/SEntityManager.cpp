#include "SEntityManager.h"
#include "SPlayerEntity.hpp"

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

int SEntityManager::addPlayerEntity()
{
	auto playerEntity = std::make_shared<SPlayerEntity>();
	entityMap.insert({ playerEntity->getState()->id, playerEntity });
	for (auto& child : playerEntity->getChildren())
	{
		entityMap.insert({ child->getState()->id, child });
	}
	return playerEntity->getState()->id;
}

void SEntityManager::updatePlayer(PlayerController playerController)
{
	auto tempEntity = entityMap.find(playerController.playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	// update left hand
	auto leftHand = playerEntity->leftHand;
	leftHand->getState()->pos = playerController.leftHandPos;
	leftHand->getState()->rotation = playerController.leftHandRotation;
	leftHand->updatedPlayerList.clear();

	// update right hand
	auto rightHand = playerEntity->rightHand;
	rightHand->getState()->pos = playerController.rightHandPos;
	rightHand->getState()->rotation = playerController.rightHandRotation;
	rightHand->updatedPlayerList.clear();
}
