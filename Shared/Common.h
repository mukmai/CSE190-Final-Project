#pragma once
#include <map>
#include <rpc/client.h>

#define PORT 8080

enum FUNCTION {
	MOVE,
	HIT,
	PLAYER_JOIN
};

// map from FUNCTION enum to string (just to make it easier to find the location
static std::map<FUNCTION, std::string> serverFunction {
{ MOVE , "move"},
{ HIT , "hit"},
{ PLAYER_JOIN , "player_join"}
};

enum EntityType
{
	ENTITY_HAND
};

MSGPACK_ADD_ENUM(EntityType);