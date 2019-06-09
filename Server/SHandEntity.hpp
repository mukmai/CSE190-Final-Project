#pragma once

#include "SBaseEntity.hpp"

class SHandEntity : public SBaseEntity
{
public:
	SHandEntity(int handIndex, int playerID) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_HAND;

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
		_state->extraData.push_back(handIndex); // extraData[HAND_INDEX] = hand index (0 = left, 1 = right)
		_state->extraData.push_back(0); // extraData[HAND_STATE] = hand state (0 = release hand, 1 = hold hand)
	};

	btRigidBody* createRigidBody() override {
		//create a dynamic rigidbody
		colShape = new btSphereShape(btScalar(0.08f));

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(0.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(bullet::fromGlm(_state->pos));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		rigidBody = new btRigidBody(rbInfo);

		// kinematic static objects (hands)
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);

		return rigidBody;
	}

	~SHandEntity() {
		delete colShape;
	};
};