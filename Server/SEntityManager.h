#pragma once
#include <unordered_map>
#include "SBaseEntity.hpp"
#include <vector>

class SEntityManager
{
public:
	std::unordered_map<int, std::shared_ptr<SBaseEntity>> entityMap;

	SEntityManager();

	std::vector<BaseState> getUpdateList(int playerID);

	void addPlayerEntity(int playerID);
};

