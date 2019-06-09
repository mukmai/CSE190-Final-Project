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
#include "SSphereEntity.hpp"
#include "PhysicsEngine.h"

SEntityManager* entityManager;
PhysicsEngine* physicsEngine;
bool terminated = false;

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

std::function<void(int, float)> movePlayerZ = [&](int playerID, float rate) {
	entityManager->movePlayerZ(playerID, rate);
};

std::function<void(int, float)> movePlayerX = [&](int playerID, float rate) {
	entityManager->movePlayerX(playerID, rate);
};

void startServer() {
	// Set up rpc server and listen to PORT
	rpc::server srv(PORT);

	auto northEntity = std::make_shared<SSphereEntity>(glm::vec3(0.1f, 0.1f, 0.3f));
	northEntity->getState()->pos = glm::vec3(0, 0, 1);
	entityManager->entityMap.insert({ northEntity->getState()->id, northEntity });

	auto eastEntity = std::make_shared<SSphereEntity>(glm::vec3(0.3f, 0.1f, 0.1f));
	eastEntity->getState()->pos = glm::vec3(2, 0, 0);
	entityManager->entityMap.insert({ eastEntity->getState()->id, eastEntity });

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

	srv.bind(serverFunction[PLAYER_JOIN], playerJoin);

	srv.bind(serverFunction[GET_UPDATE], update);

	srv.bind(serverFunction[PLAYER_INPUT], playerInput);

	srv.bind(serverFunction[PLAYER_MOVE_Z], movePlayerZ);

	srv.bind(serverFunction[PLAYER_MOVE_X], movePlayerX);

	// Blocking call to start the server: non-blocking call is srv.async_run(threadsCount);
	constexpr size_t thread_count = 2;
	srv.async_run(thread_count);
	std::cin.ignore();
	std::cout << "Listening to port: " << PORT << std::endl;
}

void updateLoop() {
	auto dt = std::chrono::milliseconds(1000 / TICK_PER_SECOND);
	while (!terminated) {
		// do data update
		physicsEngine->stepSimulation(TICK_PER_SECOND);
		std::this_thread::sleep_for(dt);
	}
}

int main()
{
	entityManager = new SEntityManager();

	physicsEngine = new PhysicsEngine(entityManager);

	std::thread reqHandleThread(startServer);
	std::thread updateLoopThread(updateLoop);
	// two threads are running simultaneously

	// synchronize threads when ending the program
	reqHandleThread.join();
	updateLoopThread.join();

	return 0;
}