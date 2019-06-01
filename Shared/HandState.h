#pragma once

#include "BaseState.h"

struct HandState : BaseState
{
	int handIndex;

	MSGPACK_DEFINE_ARRAY(
		pos.x, pos.y, pos.z,
		rotation.x, rotation.y, rotation.z, rotation.w,
		scale.x, scale.y, scale.z,
		id,
		type,
		handIndex);
};