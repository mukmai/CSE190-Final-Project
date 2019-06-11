#pragma once

#include "SBaseEntity.hpp"

class SBuildingEntity : public SBaseEntity
{
public:
	SBuildingEntity(glm::vec3 scale, glm::vec3 pos) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_BUILDING;
		_state->scale = scale;
		_state->pos = pos;
	};

	btRigidBody* createRigidBody(btDiscreteDynamicsWorld * dynamicsWorld) override {
		//create a dynamic rigidbody
		colShape = new btSphereShape(btScalar(_state->scale.x / 2));
		colShape = new btBoxShape(btVector3(
			btScalar(_state->scale.x / 2),
			btScalar(_state->scale.y / 2),
			btScalar(_state->scale.z / 2)));

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

		collideWith = COL_HEIGHT | COL_BULLET | COL_WALL;

		collisionGroup = COL_WALL;

		dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collideWith);

		rigidBody->setUserPointer(this);

		return rigidBody;
	}

	~SBuildingEntity() {
		delete colShape;
	};
};