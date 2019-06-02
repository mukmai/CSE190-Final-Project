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
    
    lastTime = ovr_getTimeInSeconds();
    newTime  = ovr_getTimeInSeconds();
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
  
  void updateState(BaseState const & state) override {
    lastTime = newTime;
    newTime  = ovr_GetTimeInSeconds();
    double deltaTime = newTime - lastTime;
  
		_state->id = state.id;

		// Translation
		_state->pos = state.pos + deltaTime * velocity;

		// Orientation
		_state->rotation = state.rotation;

		// Scale
		_state->scale = state.scale;

		// extraData
		_state->extraData = state.extraData;
	}
  
  void setVelocity(glm::vec3 vel){
    velocity = vel;
  }
  
  private:
    glm::vec3 velocity = glm::vec3(0.0f);
    
    double lastTime;
    double newTime;
};