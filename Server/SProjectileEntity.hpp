#pragma once

#include "SBaseEntity.hpp"

class SProjectileEntity : public SBaseEntity
{
public:
	SProjectileEntity(glm::vec3 scale, glm::vec3 pos, glm::quat rot, int playerID) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_PROJECTILE;
		_state->scale = scale;
		_state->pos = pos;
		_state->rotation = rot;

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
	};

	btRigidBody* createRigidBody() override {
		//create a dynamic rigidbody
		colShape = new btSphereShape(btScalar(_state->scale.x / 2));

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

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
		return rigidBody;
	}

	void setRotation(glm::quat p_quat) {
		_state->rotation = p_quat;
	}

	~SProjectileEntity() {
		delete colShape;
	};
};