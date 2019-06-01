#pragma once

#include <memory>
#include <glm/gtx/string_cast.hpp>
#include "BaseState.h"
#include "IDGenerator.h"
#include <set>

class SBaseEntity
{
public:
	//bool hasChanged; // If object state has changed during the last iteration
	std::set<int> updatedPlayerList; // stores playerID that has already updated this entity

	virtual ~SBaseEntity(); // Destroys local state and collider objects

	// All server objects must have a state to send to the client.
	virtual std::shared_ptr<BaseState> getState();

	// Initializes state struct to some sane defaults. Be sure to call on any children
	virtual void initState(bool generateId = true);

protected:
	std::shared_ptr<BaseState> _state;
};