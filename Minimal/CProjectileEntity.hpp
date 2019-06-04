#pragma once

#include "CBaseEntity.hpp"

#include <OVR_CAPI.h>

class CProjectileEntity : public CBaseEntity
{
public:
	CProjectileEntity(glm::vec3 vel) {
		// allocate state
		_state = std::make_shared<BaseState>();

		// load model and shader
		_objectModel = std::make_unique<Model>("./Resources/Models/sphere.obj", false);
		_objectShader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");
    
		velocity = vel;
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
  
  void updateState(BaseState const & state) {
    lastTime = currTime;
    currTime = ovr_GetTimeInSeconds();
    double dTime = currTime - lastTime;
    
		_state->id = state.id;

		// Translation
		//glm::mat4 rotmat = glm::toMat4(state.rotation);
		//glm::vec4 tmpvec = glm::vec4(velocity, 0.0f);
		//_state->pos = state.pos + glm::vec3(dTime) * glm::vec3(rotmat*tmpvec);

		_state->pos = state.pos;

		// Orientation
		_state->rotation = state.rotation;

		// Scale
		_state->scale = state.scale;

		// extraData
		_state->extraData = state.extraData;
	}
  
  
private:
  double lastTime;
  double currTime = ovr_GetTimeInSeconds();

  glm::vec3 velocity;
  
};