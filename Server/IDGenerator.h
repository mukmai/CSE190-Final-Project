#pragma once

#include <iostream>
#include <mutex>

/*
** ID generator class. Is a singleton, so can be called anywhere from within
** the server code. Use it to generate an ID for anything that requires state
** to be synced with clients. It will never generate an ID of zero.
*/

class IDGenerator
{
private:
	static IDGenerator * _instance;
	static std::mutex _mutex;
	int _nextId;

	IDGenerator()
	{
		_nextId = 1;
	}

public:

	static IDGenerator * getInstance()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (!_instance)
		{
			_instance = new IDGenerator();
		}
		return _instance;
	}

	uint32_t getNextId()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if ((_nextId + 1) < _nextId)
		{
			fgetc(stdin);
			exit(1);
		}
		else
		{
			return _nextId++;
		}
	}
};

