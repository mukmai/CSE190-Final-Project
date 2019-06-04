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

	void movePlayer(int playerID, float direction);

	void moveProjectile(int projID, float direction);
};

