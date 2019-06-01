#pragma once
#include "pch.h"
#include "rpc/server.h"
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "BaseState.h"
#include "PlayerController.h"
#include "Common.h"
#include "IDGenerator.h"
#include "SEntityManager.h"

SEntityManager* entityManager;

std::function<int()> playerJoin = [&] {
	int newID = entityManager->addPlayerEntity();
	std::cout << "New player joined with ID: " << newID << std::endl;
	return newID;
};

std::function<std::vector<BaseState>(int)> update = [&](int playerID) {
	auto res = entityManager->getUpdateList(playerID);
	return res;
};

std::function<void(PlayerController)> playerInput = [&](PlayerController playerController) {
	entityManager->updatePlayer(playerController);
};

int main()
{
	// Set up rpc server and listen to PORT
	rpc::server srv(PORT);
	std::cout << "Listening to port: " << PORT << std::endl;

	entityManager = new SEntityManager();

	//// Define a rpc function: auto echo(string const& s, Player& p){} (return type is deduced)
	//srv.bind("takeDamage"/*function name*/,
	//	[/*put = here if you want to capture environment by value, & by reference*/]
	//	(BaseState& state) /*function parameters*/
	//	{
	//		std::cout << "Before: Player Health: " << state.hp << std::endl;
	//		state.hp -= 1;
	//		std::cout << "After: Player Health: " << state.hp << std::endl;
	//		// return value : that will be returned back to client side
	//		return state;
	//	});

	srv.bind(serverFunction[MOVE], [](BaseState& state)
	{
		std::cout << "Before: Player Position: " << state.pos.x << " " << state.pos.y << " " << state.pos.z << " " << std::endl;
		state.pos.y -= 0.8f;
		std::cout << "After: Player Position: " << state.pos.x << " " << state.pos.y << " " << state.pos.z << " " << std::endl;
		// return value : that will be returned back to client side
		return state;
	});

	srv.bind(serverFunction[PLAYER_JOIN], playerJoin);

	srv.bind(serverFunction[GET_UPDATE], update);

	srv.bind(serverFunction[PLAYER_INPUT], playerInput);

	// Blocking call to start the server: non-blocking call is srv.async_run(threadsCount);
	srv.run();
	return 0;
}