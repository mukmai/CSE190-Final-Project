#include "SEntityManager.h"
#include "SPlayerEntity.hpp"
#include "SSphereEntity.hpp"
#include "SBoxEntity.hpp"
#include <glm/gtx/quaternion.hpp>

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

	playerEntity->getState()->pos = glm::vec3(0, 3, 0);

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
	auto head = playerEntity->head;
	head->getState()->pos = playerController.headPos;
	head->getState()->rotation = playerController.headRotation;
	head->updatedPlayerList.clear();

	// update body position
	auto body = playerEntity->body;
	body->getState()->pos = playerController.headPos;
	body->getState()->pos.y -= 0.27f;
	// extract only y rotation
	auto rotatedForward = playerController.headRotation * glm::vec3(0, 0, 1);
	glm::vec2 a = glm::vec2(0, 1);
	glm::vec2 b = glm::normalize(glm::vec2(rotatedForward.x, rotatedForward.z));
	auto angle = atan2f(a.x * b.y - a.y * b.x, glm::dot(a, b));
	auto bodyRotation = glm::angleAxis(angle, glm::vec3(0, -1.0f, 0));
	body->getState()->rotation = bodyRotation;
	body->updatedPlayerList.clear();

	//playerEntity->updatedPlayerList.clear();
}

void SEntityManager::movePlayerZ(int playerID, float rate)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	auto moveOffset = playerEntity->getForward() * rate * 0.01f;
	playerEntity->getState()->pos += moveOffset;
	playerEntity->updatedPlayerList.clear();
}

void SEntityManager::movePlayerX(int playerID, float rate)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	auto forward = playerEntity->getForward();
	auto right = glm::vec3(-forward.z, 0, forward.x);
	auto moveOffset = right * rate * 0.01f;
	playerEntity->getState()->pos += moveOffset;
	playerEntity->updatedPlayerList.clear();
}

void SEntityManager::createSphere(int playerID)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	auto rightHand = playerEntity->rightHand;

	auto moveOffset = rightHand->getPalmForward() * 1.0f;
	auto sphere = std::make_shared<SSphereEntity>(
		glm::vec3(0.2f),
		rightHand->getState()->pos + moveOffset);
	entityMap.insert({ sphere->getState()->id, sphere });
}

void SEntityManager::createBox(int playerID)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	auto moveOffset = playerEntity->getForward() * 1.0f;
	auto box = std::make_shared<SBoxEntity>(
		glm::vec3(0.2f),
		playerEntity->getState()->pos + moveOffset);
	entityMap.insert({ box->getState()->id, box });
}

void SEntityManager::increaseHandSize(int playerID)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	//auto leftHand = playerEntity->leftHand;
	//float curScale = leftHand->getState()->scale.x;
	//leftHand->getcollisionShape()->setLocalScaling(leftHand->getcollisionShape()->getLocalScaling() + bullet::fromGlm(glm::vec3(0.2f)));

	//auto rightHand = playerEntity->rightHand;
	//curScale = rightHand->getState()->scale.x;
	//rightHand->getcollisionShape()->setLocalScaling(rightHand->getcollisionShape()->getLocalScaling() + bullet::fromGlm(glm::vec3(0.2f)));

	//playerEntity->getcollisionShape()->setLocalScaling(playerEntity->getcollisionShape()->getLocalScaling() + bullet::fromGlm(glm::vec3(0.2f)));
	
	//playerEntity->getState()->pos += glm::vec3(0, 1, 0);
	//playerEntity->updatedPlayerList.clear();
}

void SEntityManager::rightHandThruster(int playerID, float rate)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	//auto rightHand = playerEntity->rightHand;

	//auto force = rightHand->getPalmForward() * THRUSTER_FORCE * rate;

	//// reduce side way speed
	//force *= glm::vec3(SIDE_THRUSTER_RATIO, 1, SIDE_THRUSTER_RATIO);

	//playerEntity->getRigidBody()->applyCentralForce(bullet::fromGlm(force));
	playerEntity->rightThrusterOn = true;
	playerEntity->rightThrusterRate = rate;
	//playerEntity->updatedPlayerList.clear();
}

void SEntityManager::leftHandThruster(int playerID, float rate)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	//auto leftHand = playerEntity->leftHand;

	//auto force = leftHand->getPalmForward() * THRUSTER_FORCE * rate;

	//// reduce side way speed
	//force *= glm::vec3(SIDE_THRUSTER_RATIO, 1, SIDE_THRUSTER_RATIO);

	//playerEntity->getRigidBody()->applyCentralForce(bullet::fromGlm(force));
	playerEntity->leftThrusterOn = true;
	playerEntity->leftThrusterRate = rate;
	//playerEntity->updatedPlayerList.clear();
}

void SEntityManager::stabilizerSwitch(int playerID)
{
	auto tempEntity = entityMap.find(playerID)->second;
	auto playerEntity = std::static_pointer_cast<SPlayerEntity>(tempEntity);

	//auto leftHand = playerEntity->leftHand;

	//auto force = leftHand->getPalmForward() * THRUSTER_FORCE * rate;

	// reduce side way speed
	//force *= glm::vec3(SIDE_THRUSTER_RATIO, 1, SIDE_THRUSTER_RATIO);

	//playerEntity->getRigidBody()->applyCentralForce(bullet::fromGlm(force));
}
