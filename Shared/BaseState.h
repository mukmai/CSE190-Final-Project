#pragma once

#include <rpc/server.h>
#include <glm/glm.hpp>

struct BaseState
{
	float hp;
	float cd;
	glm::vec3 pos;

	// rpc Macro to generate serialize code for the struct
	MSGPACK_DEFINE_ARRAY(hp, cd, pos.x, pos.y, pos.z);
};