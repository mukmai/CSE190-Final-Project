#pragma once
#include <unordered_map>
#include "SBaseEntity.hpp"
#include "PlayerController.h"
#include <vector>

class SEntityManager
{
public:
	bool gameEnd = false;

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

	void createProjectile(int playerID, int handIdx);

	void increaseHandSize(int playerID);

	void rightHandThruster(int playerID, float rate);

	void leftHandThruster(int playerID, float rate);

	void rightHandThrusterOff(int playerID);

	void leftHandThrusterOff(int playerID);

	void leftHandSwitch(int playerID);

	void rightHandSwitch(int playerID);

	void playGunshotSound(int playerID, int handIdx, int toggleOn);

	void stabilizerSwitch(int playerID);

	void hitPlayer(int playerID);

	bool getGameState();
};

