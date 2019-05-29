// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "rpc/server.h"
#include <string>
#include <iostream>

#include "BaseState.h"
#include "Common.h"

using std::string;



int main()
{
	// Set up rpc server and listen to PORT
	rpc::server srv(PORT);
	std::cout << "Listening to port: " << PORT << std::endl;

	// Define a rpc function: auto echo(string const& s, Player& p){} (return type is deduced)
	srv.bind("takeDamage"/*function name*/,
		[/*put = here if you want to capture environment by value, & by reference*/]
		(BaseState& state) /*function parameters*/
		{
			std::cout << "Before: Player Health:: " << state.hp << std::endl;
			state.hp -= 1;
			std::cout << "After: Player Health:: " << state.hp << std::endl;
			// return value : that will be returned back to client side
			return state;
		});

	// Blocking call to start the server: non-blocking call is srv.async_run(threadsCount);
	srv.run();
	return 0;
}
