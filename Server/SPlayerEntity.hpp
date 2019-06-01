#pragma once

#include "SBaseEntity.hpp"
#include "SHandEntity.hpp"

class SPlayerEntity : public SBaseEntity
{
public:
	std::shared_ptr<SHandEntity> leftHand;
	std::shared_ptr<SHandEntity> rightHand;

	SPlayerEntity() {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_PLAYER;

		leftHand = std::make_shared<SHandEntity>(0, _state->id);
		_children.push_back(leftHand);
		rightHand = std::make_shared<SHandEntity>(1, _state->id);
		_children.push_back(rightHand);
	};

	~SPlayerEntity() {
		_children.clear();
	};

	std::vector<std::shared_ptr<SBaseEntity>> getChildren() override
	{
		return _children;
	}

private:
	std::vector<std::shared_ptr<SBaseEntity>> _children;
};