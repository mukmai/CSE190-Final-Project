#pragma once

#include "SBaseEntity.hpp"

class SBodyEntity : public SBaseEntity
{
public:
	SBodyEntity(int playerID) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_BODY;

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
	};

	~SBodyEntity() {};
};