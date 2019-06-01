#pragma once

#include "BaseState.h"
#include "shader.h"
#include "model.h"
#include <glm/gtx/quaternion.hpp>

/*
** This is an interface that any graphics objects on the client side must be
** derived from. Do not instantiate objects of this class.
**
** E.g.
** class CPlayer : public CBaseEntity {..}
** etc.
*/
class CBaseEntity {
public:


	// Base render function, affects all entities. If special handling is
	// needed, override this in the child.
	virtual void render(const glm::mat4& projection, const glm::mat4& view) {
		_objectShader->use();
		setUniforms(projection, view);
		_objectModel->draw(*_objectShader);
	}

	// Set uniforms before rendering
	virtual void setUniforms(const glm::mat4& projection, const glm::mat4& view) {
		// Compute model matrix based on state: t * r * s
		const auto t = glm::translate(glm::mat4(1.0f), _state->pos);
		const auto r = glm::toMat4(_state->rotation);
		const auto s = glm::scale(glm::mat4(1.0f), _state->scale);
		glm::mat4 model = t * r * s;

		_objectShader->setMat4("projection", projection);
		_objectShader->setMat4("model", model);
		_objectShader->setMat4("view", view);
	}

	// Every child must override this if they carry additional state
	virtual void updateState(BaseState const & state) {
		_state->id = state.id;

		// Translation
		_state->pos = state.pos;

		// Orientation
		_state->rotation = state.rotation;

		// Scale
		_state->scale = state.scale;
	}

	int getID() {
		return _state->id;
	}

	std::shared_ptr<BaseState> const& getState() {
		return _state;
	}

protected:
	// State pointer for object
	std::shared_ptr<BaseState> _state;

	// Model
	std::unique_ptr<Model> _objectModel;

	// Shader program for object
	std::unique_ptr<Shader> _objectShader;
};