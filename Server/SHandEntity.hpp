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

		//_state->scale = glm::vec3(0.04f, 0.04f, 1.0f);

		_state->extraData.push_back(playerID); // extraData[PLAYER_ID] = player ID
		_state->extraData.push_back(handIndex); // extraData[HAND_INDEX] = hand index (0 = left, 1 = right)
		_state->extraData.push_back(0); // extraData[HAND_STATE]  = hand state (0 = release hand, 1 = hold hand)
		_state->extraData.push_back(0); // extraData[THRUSTER_ON] = thrust status (0 = off, 1 = on)
		//_state->extraData.push_back(0); // extraData[FIRED_GUN] = sound status (0 = off, 1 = on)
	};

	btRigidBody* createRigidBody(btDiscreteDynamicsWorld * dynamicsWorld) override {
		//create a dynamic rigidbody
		colShape = new btSphereShape(btScalar(0.03f));
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

		int collideWith = COL_BULLET | COL_WALL;

		dynamicsWorld->addRigidBody(rigidBody, COL_BODY, collideWith);

		return rigidBody;
	}

	btGhostObject* createGhostObject() override {
		auto ghostObject = new btGhostObject();
		ghostObject->setCollisionShape(new btSphereShape(btScalar(0.08f)));

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(bullet::fromGlm(_state->pos));

		ghostObject->setWorldTransform(startTransform);

		return ghostObject;
	}

	glm::vec3 getPalmForward() {
		return _state->rotation * glm::vec3(-1 + _state->extraData[HAND_INDEX] * 2, 0, 0);
	}

	~SHandEntity() {
		delete colShape;
	};
};