#pragma once

#include "SBaseEntity.hpp"

class SBodyEntity : public SBaseEntity
{
public:
	SBodyEntity(int playerID) {
		// Allocated a state struct and initialize (Modify if using other state)
		_state = std::make_shared<BaseState>();

		// Base defaults
		SBaseEntity::initState();
		_state->type = ENTITY_BODY;

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
	};

	btRigidBody* createRigidBody(btDiscreteDynamicsWorld * dynamicsWorld) override {
		//create a dynamic rigidbody
		colShape = new btSphereShape(btScalar(0.1f));
		//colShape = new btBoxShape(btVector3(btScalar(0.02f), btScalar(0.02f), btScalar(0.5f)));

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
		//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);

		collideWith = COL_BULLET | COL_WALL;

		collisionGroup = COL_BODY;

		dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collideWith);

		rigidBody->setUserPointer(this);

		return rigidBody;
	}

	~SBodyEntity() {};
};