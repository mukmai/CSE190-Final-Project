#pragma once
#include <glm/mat4x2.hpp>
class Renderable
{
public:
	Renderable();
	virtual void render(const glm::mat4& projection, const glm::mat4& view) = 0;
};

