#pragma once

#include <rpc/server.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct PlayerController
{
	int playerID;
	glm::vec3 headPos;
	glm::quat headRotation;
	glm::vec3 leftHandPos;
	glm::quat leftHandRotation;
	glm::vec3 rightHandPos;
	glm::quat rightHandRotation;

	MSGPACK_DEFINE_ARRAY(
		playerID,
		headPos.x, headPos.y, headPos.z,
		headRotation.x, headRotation.y, headRotation.z, headRotation.w,
		leftHandPos.x, leftHandPos.y, leftHandPos.z,
		leftHandRotation.x, leftHandRotation.y, leftHandRotation.z, leftHandRotation.w,
		rightHandPos.x, rightHandPos.y, rightHandPos.z,
		rightHandRotation.x, rightHandRotation.y, rightHandRotation.z, rightHandRotation.w
		);
};