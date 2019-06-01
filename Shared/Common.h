#pragma once
#include <map>
#include <rpc/client.h>

#define PORT 8080

enum FUNCTION {
	MOVE,
	HIT,
	PLAYER_JOIN,
	GET_UPDATE,
	TEST,
	PLAYER_INPUT
};

// map from FUNCTION enum to string (just to make it easier to find the location
static std::map<FUNCTION, std::string> serverFunction {
{ MOVE , "move"},
{ HIT , "hit"},
{ PLAYER_JOIN , "player_join"},
{ GET_UPDATE , "get_update"},
{ TEST , "test"},
{ PLAYER_INPUT , "player_input"}
};

enum EntityType
{
	ENTITY_HAND,
	ENTITY_PLAYER
};

MSGPACK_ADD_ENUM(EntityType);