#pragma once

#include "SBaseEntity.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

		glm::vec3 initVel = glm::vec4(0.0f, 0.0f, -20.0f, 0.0f);
		initVel = glm::toMat4(_state->rotation) * glm::vec4(initVel, 0.0f);

		// Calculate the initial velocity of the projectile
		rigidBody->setLinearVelocity(bullet::fromGlm(initVel));

		rigidBody->setFriction(0);

		collideWith = COL_WALL | COL_BODY;

		collisionGroup = COL_BULLET;

		dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collideWith);

		// STEP: Turn off gravity for this entity
		rigidBody->setGravity(btVector3(0, 0, 0));

		rigidBody->setUserPointer(this);

		return rigidBody;
	}

	void setRotation(glm::quat p_quat) {
		_state->rotation = p_quat;
	}

	~SProjectileEntity() {
		delete colShape;
	};
};