#pragma once

#include "CBaseEntity.hpp"

class CBodyEntity : public CBaseEntity
{
public:
	CBodyEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/body.obj", false);

		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");
	}

	void setUniforms(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		CBaseEntity::setUniforms(projection, view, eyePos);
		_objectShader->setFloat("shininess", 0.5f);

		// change to global position
		const auto t = glm::translate(glm::mat4(1.0f), _state->pos);
		const auto r = glm::toMat4(_state->rotation);
		const auto s = glm::scale(glm::mat4(1.0f), _state->scale);
		glm::mat4 model = t * r * s; // local position
		auto playerState = EntityManager::getInstance().getEntity(_state->extraData[PLAYER_ID])->getState();
		auto globalPlayerTranslation = glm::translate(glm::mat4(1), playerState->pos);
		auto globalPlayerRotation = glm::mat4_cast(playerState->rotation);
		model = globalPlayerTranslation * globalPlayerRotation * model; // global position

		_objectShader->setMat4("model", model);
	}

	void render(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		// only render body when it is not this player's body
		if (EntityManager::getInstance().getPlayerID() != _state->extraData[PLAYER_ID]) {
			_objectShader->use();
			setUniforms(projection, view, eyePos);
			_objectModel->draw(*_objectShader);
		}
	}
};