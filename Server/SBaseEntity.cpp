#include "SBaseEntity.hpp"

SBaseEntity::~SBaseEntity()
{
	_state = nullptr;
}

std::shared_ptr<BaseState> SBaseEntity::getState()
{
	return _state;
}

void SBaseEntity::initState(bool generateId)
{
}
