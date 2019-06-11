#pragma once

#include "CBaseEntity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class CProjectileEntity : public CBaseEntity
{
public:
	CProjectileEntity() {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/sphere.obj", false);

		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");

		//aps = new AParticleSystem();
		//aps->setSound("Resources/Audio/SoundEffects/Thruster1.wav");
		//aps->bPlaySound = false;
		//aps->vecSpawnDir = glm::vec3(0.0f, 0.0f, 4.0f);
	}

	void setUniforms(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		CBaseEntity::setUniforms(projection, view, eyePos);
		_objectShader->setFloat("shininess", 0.5f);
	}

	void render(const glm::mat4& projection, const glm::mat4& view, glm::vec3 eyePos) override {
		_objectShader->use();
		setUniforms(projection, view, eyePos);
		_objectModel->draw(*_objectShader);

		// STEP: Update the particle system and the audio associated with it.
		//       Then render it.
		//if (aps != NULL) {
		//	aps->update(eyePos);
		//	aps->update3DAudio(glm::inverse(view));
		//	aps->render(projection, view);
		//}
		
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
		glm::vec3 velocity = _state->pos - lastPos;
		float speed = glm::abs(glm::length(velocity));
		//std::cout << "Projectile ID: " << _state->id << " / Speed: (" << speed << ")" << std::endl;

		//if (speed > 0.05f) {
		//	if (aps != NULL) {
		//		aps->playPS();
		//	}
		//}
		//else {
		//	aps->stopPS();
		//	delete(aps);
		//	aps = NULL;
		//}

		//
		//if (aps != NULL) {
		//	//aps->vecSpawnDir = _state->rotation * glm::vec4(0.0f, 0.0f, 4.0f, 0.0f);
		//	aps->vecSpawnDir = velocity;

		//	lastPos = _state->pos;

		//	glm::mat4 modelMat = glm::toMat4(_state->rotation);
		//	modelMat[3] = glm::vec4(_state->pos, 1.0f);
		//	aps->matModel = modelMat;
		//}

	}

protected:
	glm::vec3 lastPos = glm::vec3(0.0f);

	//AParticleSystem * aps;
};