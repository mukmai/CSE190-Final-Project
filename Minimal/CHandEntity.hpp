#pragma once

#include "CBaseEntity.hpp"

class CHandEntity : public CBaseEntity
{
public:
	CHandEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/sphere.obj", false);
		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");
	}
};