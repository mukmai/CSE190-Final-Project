#pragma once

#include "SBaseEntity.hpp"

class SPlayerEntity : public SBaseEntity
{
public:
	SPlayerEntity() {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_HAND;
	};

	~SPlayerEntity() {};
};