#pragma once

#include "SBaseEntity.hpp"

class SHeadEntity : public SBaseEntity
{
public:
	SHeadEntity(int playerID) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_HEAD;

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
	};

	~SHeadEntity() {};
};