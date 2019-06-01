#pragma once

#include "CBaseEntity.hpp"

class CPlayerEntity : public CBaseEntity
{
public:
	CPlayerEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();
	}

	void render(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {

	}

	void setUniforms(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
	}
};