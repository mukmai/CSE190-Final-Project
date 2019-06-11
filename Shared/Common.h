#pragma once
#include <map>
#include <rpc/client.h>

#define PORT 8008
#define TICK_PER_SECOND 60

#define THRUSTER_FORCE 230.0f
#define SIDE_THRUSTER_RATIO 0.3f

enum FUNCTION {
	MOVE,
	HIT,
	PLAYER_JOIN,
	GET_UPDATE,
	TEST,
	PLAYER_INPUT,
	PLAYER_MOVE_Z,
	PLAYER_MOVE_X,
	PLAYER_BUTTON_A,
	PLAYER_BUTTON_X,
	PLAYER_BUTTON_Y,
	PLAYER_BUTTON_B,
	PLAYER_LEFT_THRUSTER,
	PLAYER_RIGHT_THRUSTER
};

// map from FUNCTION enum to string (just to make it easier to find the location
static std::map<FUNCTION, std::string> serverFunction {
{ MOVE , "move"},
{ HIT , "hit"},
{ PLAYER_JOIN , "player_join"},
{ GET_UPDATE , "get_update"},
{ TEST , "test"},
{ PLAYER_INPUT , "player_input"},
{ PLAYER_MOVE_Z , "player_move_z"},
{ PLAYER_MOVE_X , "player_move_x"},
{ PLAYER_BUTTON_A , "player_button_a"},
{ PLAYER_BUTTON_X , "player_button_x"},
{ PLAYER_BUTTON_Y , "player_button_y"},
{ PLAYER_BUTTON_B , "player_button_b"},
{ PLAYER_LEFT_THRUSTER , "player_left_thruster"},
{ PLAYER_RIGHT_THRUSTER , "player_right_thruster"}
};

enum EntityType
{
	ENTITY_HAND,
	ENTITY_PLAYER,
	ENTITY_HEAD,
	ENTITY_BODY,
	ENTITY_SPHERE,
	ENTITY_BOX
};

MSGPACK_ADD_ENUM(EntityType);

enum ColliderType
{
	COLLIDER_SPHERE,
	COLLIDER_BOX
};

MSGPACK_ADD_ENUM(ColliderType);

enum HandExtraData
{
	PLAYER_ID,
	HAND_INDEX,
	HAND_STATE
};