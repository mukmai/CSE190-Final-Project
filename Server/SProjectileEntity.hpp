#pragma once

#include "SBaseEntity.hpp"

class SProjectileEntity : public SBaseEntity
{
public:
	SProjectileEntity() {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_PROJECTILE;
	};

	~SProjectileEntity() {};
};