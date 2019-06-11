#pragma once

#include "ImageGUI.hpp"

class GameOverHUD : public ImageGUI
{
protected:
	unsigned int loseTextureID;
	float healthPercentage;

public:

	bool gameEnd = false;

	GameOverHUD(float width, float height) : ImageGUI(width, height) {
		textureID = model->TextureFromFile("youWin.png", "./Resources/Textures/", false);
		_quad_size = glm::vec2(400, 70);
		_quad_pos = glm::vec2(width / 2, height / 1.7f);
		loseTextureID = model->TextureFromFile("youLose.png", "./Resources/Textures/", false);
	}

	virtual void render(float offset) {
		if (!gameEnd) return;
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		_quadShader->use();

		// Compute model matrix based on state: t * r * s
		const auto t = glm::translate(glm::mat4(1.0f), glm::vec3(((_quad_pos + glm::vec2(offset, 0)) / _win_size * 2.0f) - 1.0f, 0));
		const auto s = glm::scale(glm::mat4(1.0f), glm::vec3(_quad_size / _win_size, 1));

		auto model = t * s;

		// Pass model matrix into shader
		_quadShader->setMat4("u_model", model);

		glBindVertexArray(VAO);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		if (healthPercentage == 0) {
			glBindTexture(GL_TEXTURE_2D, loseTextureID);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, textureID);
		}

		// Set our "renderedTexture" sampler to use Texture Unit 0
		_quadShader->setInt("rgbTexture", 0);

		// Draw the triangles 
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	void updateHealth(float percentage) {
		healthPercentage = percentage;
	}
};