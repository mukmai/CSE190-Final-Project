#pragma once

#include "SBaseEntity.hpp"

class SSphereEntity : public SBaseEntity
{
public:
	SSphereEntity(glm::vec3 scale, glm::vec3 pos) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_SPHERE;
		_state->scale = scale;
		_state->pos = pos;
	};

	btRigidBody* createRigidBody(btDiscreteDynamicsWorld * dynamicsWorld) override {
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

		int collideWith = COL_BODY | COL_BULLET | COL_WALL;

		dynamicsWorld->addRigidBody(rigidBody, COL_WALL, collideWith);

		return rigidBody;
	}

	~SSphereEntity() {
		delete colShape;
	};
};