#include "SEntityManager.h"
#include "SPlayerEntity.hpp"
#include "SProjectileEntity.hpp"

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
	playerList.push_back(playerEntity->getState()->id);

	// TODO: this is just a temporary offset changing for player 1 and 2
	if (playerList.size() == 2) {
		playerEntity->getState()->pos = glm::vec3(0, 0, 2.0f);
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

	// update head position
	//playerEntity->getState()->pos = playerController.headPos;
	//playerEntity->getState()->rotation = playerController.headRotation;
}

void SEntityManager::movePlayer(int playerID, float direction)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	playerEntity->getState()->pos.z += direction * 0.01f;
	playerEntity->updatedPlayerList.clear();
}

void SEntityManager::moveProjectile(int projID, float direction)
{
	auto tempEntity = entityMap.find(projID)->second;
	auto projEntity = std::static_pointer_cast<SProjectileEntity>(tempEntity);

	projEntity->getState()->pos.z += direction * 0.01f;
}
