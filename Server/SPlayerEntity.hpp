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

	bool leftThrusterOn = false;
	bool rightThrusterOn = false;
	float leftThrusterRate = 0;
	float rightThrusterRate = 0;

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

	btRigidBody* createRigidBody() override {
		//create a dynamic rigidbody
		colShape = new btCapsuleShape(btScalar(0.25f), btScalar(1.6f));
		//colShape = new btSphereShape(btScalar(0.5f));
		//colShape = new btBoxShape(btVector3(btScalar(0.02f), btScalar(0.02f), btScalar(0.5f)));

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(46.f);

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
		//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		//rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
		rigidBody->setAngularFactor(0);
		return rigidBody;
	}

	void updateBody() {
		
		if (leftThrusterOn) {
			auto force = leftHand->getPalmForward() * THRUSTER_FORCE * leftThrusterRate;

			// reduce side way speed
			force *= glm::vec3(SIDE_THRUSTER_RATIO, 1, SIDE_THRUSTER_RATIO);
			rigidBody->applyCentralForce(bullet::fromGlm(force));
			leftThrusterOn = false;
		}
		if (rightThrusterOn) {
			auto force = rightHand->getPalmForward() * THRUSTER_FORCE * rightThrusterRate;

			// reduce side way speed
			force *= glm::vec3(SIDE_THRUSTER_RATIO, 1, SIDE_THRUSTER_RATIO);
			rigidBody->applyCentralForce(bullet::fromGlm(force));
			rightThrusterOn = false;
		}
	}

private:
	std::vector<std::shared_ptr<SBaseEntity>> _children;
};