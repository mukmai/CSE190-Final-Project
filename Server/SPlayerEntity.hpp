#pragma once

#include "SBaseEntity.hpp"
#include "SHandEntity.hpp"
#include "SHeadEntity.hpp"
#include "SBodyEntity.hpp"

class SPlayerEntity : public SBaseEntity
{
public:
	std::shared_ptr<SHandEntity> leftHand;
	std::shared_ptr<SHandEntity> rightHand;
	std::shared_ptr<SHeadEntity> head;
	std::shared_ptr<SBodyEntity> body;

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
		head = std::make_shared<SHeadEntity>(_state->id);
		_children.push_back(head);
		body = std::make_shared<SBodyEntity>(_state->id);
		_children.push_back(body);
	};

	~SPlayerEntity() {
		_children.clear();
	};

	std::vector<std::shared_ptr<SBaseEntity>> getChildren() override
	{
		return _children;
	}

	glm::vec3 getForward() {
		return body->getState()->rotation * glm::vec3(0, 0, -1);
	}

private:
	std::vector<std::shared_ptr<SBaseEntity>> _children;
};