#pragma once

#include "ImageGUI.hpp"

class HealthHUD : public ImageGUI
{
protected:
	unsigned int textTextureID;
	glm::vec2 text_quad_size;
	glm::vec2 text_quad_pos;
	unsigned int healthTextureID;
	glm::vec2 health_quad_size;
	glm::vec2 health_quad_pos;
public:

	float healthPercentage = 1.0f;

	HealthHUD(float width, float height) : ImageGUI(width, height) {
		textureID = model->TextureFromFile("light_blue.jpg", "./Resources/Textures/", false);
		_quad_size = glm::vec2(300, 50);
		_quad_pos = glm::vec2(200, 150);
		textTextureID = model->TextureFromFile("health.png", "./Resources/Textures/", false);
		text_quad_size = _quad_size / 2.0f;
		text_quad_pos = _quad_pos + glm::vec2(-_quad_size.x / 4, _quad_size.y / 4 * 3);
		healthTextureID = model->TextureFromFile("green.jpg", "./Resources/Textures/", false);
		health_quad_size = glm::vec2((_quad_size.x - 20) * healthPercentage, _quad_size.y - 15);
		health_quad_pos = _quad_pos - glm::vec2((_quad_size.x - 20) * (1 - healthPercentage) / 2.0f, 0);
	}

	virtual void render(float offset) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		_quadShader->use();

		// Compute model matrix based on state: t * r * s
		auto t = glm::translate(glm::mat4(1.0f), glm::vec3(((_quad_pos + glm::vec2(offset, 0)) / _win_size * 2.0f) - 1.0f, 0));
		auto s = glm::scale(glm::mat4(1.0f), glm::vec3(_quad_size / _win_size, 1));
		auto model = t * s;

		// Pass model matrix into shader
		_quadShader->setMat4("u_model", model);

		glBindVertexArray(VAO);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set our "renderedTexture" sampler to use Texture Unit 0
		_quadShader->setInt("rgbTexture", 0);

		// Draw the triangles 
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glBindVertexArray(0);

		// Compute model matrix based on state: t * r * s
		t = glm::translate(glm::mat4(1.0f), glm::vec3(((text_quad_pos + glm::vec2(offset, 0)) / _win_size * 2.0f) - 1.0f, 0));
		s = glm::scale(glm::mat4(1.0f), glm::vec3(text_quad_size / _win_size, 1));
		model = t * s;

		// Pass model matrix into shader
		_quadShader->setMat4("u_model", model);

		glBindVertexArray(VAO);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, textTextureID);

		// Set our "renderedTexture" sampler to use Texture Unit 0
		_quadShader->setInt("rgbTexture", 0);

		// Draw the triangles 
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glBindVertexArray(0);

		// Compute model matrix based on state: t * r * s
		t = glm::translate(glm::mat4(1.0f), glm::vec3(((health_quad_pos + glm::vec2(offset, 0)) / _win_size * 2.0f) - 1.0f, 0));
		s = glm::scale(glm::mat4(1.0f), glm::vec3(health_quad_size / _win_size, 1));
		model = t * s;

		// Pass model matrix into shader
		_quadShader->setMat4("u_model", model);

		glBindVertexArray(VAO);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, healthTextureID);

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
		health_quad_size = glm::vec2((_quad_size.x - 20) * healthPercentage, _quad_size.y - 15);
		health_quad_pos = _quad_pos - glm::vec2((_quad_size.x - 20) * (1 - healthPercentage) / 2.0f, 0);
	}
};