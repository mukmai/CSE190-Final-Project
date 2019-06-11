#pragma once
#include <unordered_map>
#include "SBaseEntity.hpp"
#include "PlayerController.h"
#include <vector>

class SEntityManager
{
public:
	std::unordered_map<int, std::shared_ptr<SBaseEntity>> entityMap;

	std::vector<int> playerList;

	SEntityManager();

	std::vector<BaseState> getUpdateList(int playerID);

	int addPlayerEntity();

	void updatePlayer(PlayerController playerController);

	void movePlayerZ(int playerID, float rate);

	void movePlayerX(int playerID, float rate);

	void createSphere(int playerID);

	void createBox(int playerID);

	void increaseHandSize(int playerID);

	void rightHandThruster(int playerID, float rate);

	void leftHandThruster(int playerID, float rate);

	void rightHandThrusterOff(int playerID);

	void leftHandThrusterOff(int playerID);

	void stabilizerSwitch(int playerID);

};

