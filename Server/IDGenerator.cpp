#include "IDGenerator.h"

std::mutex IDGenerator::_mutex;
IDGenerator* IDGenerator::_instance = NULL;