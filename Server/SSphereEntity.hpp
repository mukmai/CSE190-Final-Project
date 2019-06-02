#pragma once

#include "SBaseEntity.hpp"

class SSphereEntity : public SBaseEntity
{
public:
	SSphereEntity(glm::vec3 scale) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_SPHERE;
		_state->scale = scale;
	};

	~SSphereEntity() {};
};