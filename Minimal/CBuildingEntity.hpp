#pragma once

#include "CBaseEntity.hpp"

class CBuildingEntity : public CBaseEntity
{
public:
	CBuildingEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/buildingBox.obj", false);

		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");
	}

	void setUniforms(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		CBaseEntity::setUniforms(projection, view, eyePos);
		_objectShader->setFloat("shininess", 0.8f);
	}
};