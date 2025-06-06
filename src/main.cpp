#include "Server.hpp"
#include "World.hpp"
#include "script/ScriptedThing.hpp"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>

#include "unistd.h"

int main()
{
	ScriptedThing::InitLua();

	int ip = 23;

	boost::asio::io_service io_service;
	tcp::endpoint endpoint(tcp::v4(), ip);

	Server server(ip, io_service, endpoint);
	World world;

	// std::freopen( "Log.txt", "w+", stderr ); // Start logging

	for (;;)
	{
		// std::this_thread::sleep_for( std::chrono::seconds(1) );

		server.doUpdate(world);
		world.doUpdate();
	}

	return 0;
}
