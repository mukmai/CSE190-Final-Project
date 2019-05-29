#pragma once

#include <rpc/server.h>

struct BaseState
{
	float hp;
	float cd;
	float pos[3];

	// rpc Macro to generate serialize code for the struct
	MSGPACK_DEFINE_ARRAY(hp, cd, pos);
};