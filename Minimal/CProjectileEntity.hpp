#pragma once

#include "CBaseEntity.hpp"

class CProjectileEntity : public CBaseEntity
{
public:
	CProjectileEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/sphere.obj", false);

		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");
	}

	void setUniforms(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		CBaseEntity::setUniforms(projection, view, eyePos);
		_objectShader->setFloat("shininess", 0.5f);
	}

	// Every child must override this if they carry additional state
	void updateState(BaseState const & state) override {
		_state->id = state.id;

		// Translation
		_state->pos = state.pos;

		// Orientation
		_state->rotation = state.rotation;

		// Scale
		_state->scale = state.scale;

		// extraData
		_state->extraData = state.extraData;

		//std::cout << "Projectile ID: " << _state->id << " / Position: (" << _state->pos.x << "," << _state->pos.y << "," << _state->pos.z << ")" << std::endl;
	}

protected:
	glm::vec3 lastPos = glm::vec3(0.0f);
};