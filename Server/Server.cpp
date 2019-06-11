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
std::mutex mtx;

std::function<int()> playerJoin = [&] {
	mtx.lock();
	int newID = entityManager->addPlayerEntity();
	std::cout << "New player joined with ID: " << newID << std::endl;
	mtx.unlock();
	return newID;
};

std::function<std::vector<BaseState>(int)> update = [&](int playerID) {
	mtx.lock();
	auto res = entityManager->getUpdateList(playerID);
	mtx.unlock();
	return res;
};

std::function<void(PlayerController)> playerInput = [&](PlayerController playerController) {
	mtx.lock();
	entityManager->updatePlayer(playerController);
	mtx.unlock();
};

std::function<void(int, float)> movePlayerZ = [&](int playerID, float rate) {
	mtx.lock();
	entityManager->movePlayerZ(playerID, rate);
	mtx.unlock();
};

std::function<void(int, float)> movePlayerX = [&](int playerID, float rate) {
	mtx.lock();
	entityManager->movePlayerX(playerID, rate);
	mtx.unlock();
};

std::function<void(int)> playerButtonA = [&](int playerID) {
	mtx.lock();
	entityManager->createSphere(playerID);
	mtx.unlock();
};

std::function<void(int)> playerButtonX = [&](int playerID) {
	mtx.lock();
	entityManager->createBox(playerID);
	mtx.unlock();
};

std::function<void(int)> playerButtonY = [&](int playerID) {
	//entityManager->leftHandThruster(playerID);
};

std::function<void(int)> playerButtonB = [&](int playerID) {
	//entityManager->rightHandThruster(playerID);
};

std::function<void(int, float)> playerLeftThruster = [&](int playerID, float rate) {
	mtx.lock();
	entityManager->leftHandThruster(playerID, rate);
	mtx.unlock();
};

std::function<void(int, float)> playerRightThruster = [&](int playerID, float rate) {
	mtx.lock();
	entityManager->rightHandThruster(playerID, rate);
	mtx.unlock();
};

std::function<void(int)> playerLeftThrusterOff = [&](int playerID) {
	mtx.lock();
	entityManager->leftHandThrusterOff(playerID);
	mtx.unlock();
};

std::function<void(int)> playerRightThrusterOff = [&](int playerID) {
	mtx.lock();
	entityManager->rightHandThrusterOff(playerID);
	mtx.unlock();
};

void startServer() {
	// Set up rpc server and listen to PORT
	rpc::server srv(PORT);

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

	srv.bind(serverFunction[PLAYER_BUTTON_A], playerButtonA);

	srv.bind(serverFunction[PLAYER_BUTTON_X], playerButtonX);

	srv.bind(serverFunction[PLAYER_BUTTON_Y], playerButtonY);

	srv.bind(serverFunction[PLAYER_BUTTON_B], playerButtonB);

	srv.bind(serverFunction[PLAYER_LEFT_THRUSTER], playerLeftThruster);

	srv.bind(serverFunction[PLAYER_RIGHT_THRUSTER], playerRightThruster);

	srv.bind(serverFunction[PLAYER_LEFT_THRUSTER_OFF], playerLeftThrusterOff);

	srv.bind(serverFunction[PLAYER_RIGHT_THRUSTER_OFF], playerRightThrusterOff);

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
		mtx.lock();
		physicsEngine->stepSimulation(TICK_PER_SECOND);
		mtx.unlock();
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