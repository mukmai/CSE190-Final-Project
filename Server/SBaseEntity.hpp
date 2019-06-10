#pragma once

#include <memory>
#include <glm/gtx/string_cast.hpp>
#include "BaseState.h"
#include "IDGenerator.h"
#include <set>
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class SBaseEntity
{
public:
	// stores playerID that has already updated this entity
	std::set<int> updatedPlayerList; 

	virtual ~SBaseEntity(); // Destroys local state and collider objects

	// All server objects must have a state to send to the client.
	virtual std::shared_ptr<BaseState> getState();

	// Initializes state struct to some sane defaults. Be sure to call on any children
	virtual void initState(bool generateId = true);

	// return all children entities
	virtual	std::vector<std::shared_ptr<SBaseEntity>> getChildren();

	virtual btRigidBody* createRigidBody();

	virtual btGhostObject* createGhostObject();

protected:
	std::shared_ptr<BaseState> _state;

	btRigidBody* rigidBody;

	btCollisionShape* colShape;
};

namespace bullet
{
	inline glm::vec3 toGlm(const btVector3& bt) {
		glm::vec3 res;
		res.x = (float)bt.getX();
		res.y = (float)bt.getY();
		res.z = (float)bt.getZ();
		return res;
	}

	inline btVector3 fromGlm(const glm::vec3& v) {
		return btVector3(v.x, v.y, v.z);
	}

	inline glm::quat toGlm(const btQuaternion& bt) {
		glm::quat res;
		res.x = (float)bt.getX();
		res.y = (float)bt.getY();
		res.z = (float)bt.getZ();
		res.w = (float)bt.getW();
		return res;
	}

	inline btQuaternion fromGlm(const glm::quat& v) {
		return btQuaternion(v.x, v.y, v.z);
	}
}