#pragma once

#include "CBaseEntity.hpp"

#include "AParticleSystem.h"

class CHandEntity : public CBaseEntity
{
public:
	AParticleSystem * aps;

	bool bPlayPS = false;

	CHandEntity(int handIndex) {
		// allocate state
		_state = std::make_shared<BaseState>();

		aps = new AParticleSystem();
		aps->setSound("Resources/Audio/SoundEffects/Thruster1.wav");

		// load model and shader
		if (handIndex == 0) {
			_objectModel = std::make_unique<Model>("./Resources/Models/left_hand.obj", false);
			aps->vecSpawnDir = glm::vec3(5.0f, 0.0f, 0.0f);
		}
		else {
			_objectModel = std::make_unique<Model>("./Resources/Models/right_hand.obj", false);
			aps->vecSpawnDir = glm::vec3(-5.0f, 0.0f, 0.0f);
		}

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

		// XXX: Hack to store the player's last position and rotation
		// STEP: Update attached sound and particle systems
		aps->matModel = model;

		_objectShader->setMat4("model", model);
	}

	void render(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		_objectShader->use();
		setUniforms(projection, view, eyePos);
		_objectModel->draw(*_objectShader);

		// STEP: Update the particle system and the audio associated with it.
		//       Then render it.
		aps->update(eyePos);
		aps->update3DAudio(glm::inverse(view));
		aps->render(projection, view);
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

		//std::cout << "Checking the extraData for a hand entity for THRUSTER_ON" << std::endl;
		//std::cout << "Size of extraData: " << _state->extraData.size() << std::endl;
		std::cout << "extraData.at(THRUSTER_ON): " << _state->extraData.at(THRUSTER_ON) << std::endl;
		if (_state->extraData.at(THRUSTER_ON) == 1) {
			aps->playPS();
		}
		else {
			aps->stopPS();
		}
		
	}
};