#pragma once

#include "SBaseEntity.hpp"

class SHandEntity : public SBaseEntity
{
public:
	SHandEntity(int handIndex) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_HAND;

		_state->extraData.push_back(handIndex); // extraData[0] = hand index (0 = left, 1 = right)
		_state->extraData.push_back(0); // extraData[1] = hand state (0 = release hand, 1 = hold hand)
	};

	~SHandEntity() {};
};