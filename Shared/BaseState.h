#pragma once

#include <rpc/server.h>
#include "Common.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct BaseState
{
	glm::vec3 pos;
	glm::quat rotation;
	glm::vec3 scale;
	int id;
	EntityType type;
	std::vector<int> extraData; // any special type of information will store in this vector
	ColliderType collider;
	glm::vec3 colliderScale;

	// rpc Macro to generate serialize code for the struct
	MSGPACK_DEFINE_ARRAY(
		pos.x, pos.y, pos.z,
		rotation.x, rotation.y, rotation.z, rotation.w,
		scale.x, scale.y, scale.z,
		id,
		type,
		extraData,
		collider,
		colliderScale.x, colliderScale.y, colliderScale.z
		);
};