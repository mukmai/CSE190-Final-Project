#pragma once

#include "model.h"
#include "shader.h"
#include "Texture.hpp"

static const GLfloat g_quad_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

class ImageGUI
{
protected:
	std::unique_ptr<Shader> _quadShader;
	unsigned int textureID;

	glm::vec2 _win_size;
	glm::vec2 _quad_size;
	glm::vec2 _quad_pos;
	float _quad_rotation;
	GLuint VAO, VBO;
	Model* model;

public:

	ImageGUI(float width, float height) {
		init(width, height);
	}

	virtual void init(float width, float height) {
		_quadShader = std::make_unique<Shader>("./Resources/Shaders/imageGUI.vert", "./Resources/Shaders/imageGUI.frag");

		updateWindowSize(width, height);
		_quad_rotation = 0.0f;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3, // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			3 * sizeof(GLfloat), // stride
			(void*)0 // array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//_quad_size = glm::vec2(100, 100);
		model = new Model("./Resources/Models/sphere.obj", false);
		//textureID = model->TextureFromFile("health.png", "./Resources/Textures/", false);
	}

	virtual void updateWindowSize(float width, float height) {
		_win_size = glm::vec2(width, height);
		_quad_size = glm::vec2(300, 100);
		_quad_pos = glm::vec2(width/2, height/2);
	}

	virtual void updateRotation(float degree) {
		_quad_rotation = degree;
	}

	virtual void render(float offset) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		_quadShader->use();

		// Compute model matrix based on state: t * r * s
		const auto t = glm::translate(glm::mat4(1.0f), glm::vec3(((_quad_pos + glm::vec2(offset,0)) / _win_size * 2.0f) - 1.0f, 0));
		const auto s = glm::scale(glm::mat4(1.0f), glm::vec3(_quad_size / _win_size, 1));

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
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
};

