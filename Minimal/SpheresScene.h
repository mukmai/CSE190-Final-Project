#pragma once
#include <glm/mat4x2.hpp>
#include <memory>
#include "shader.h"
#include <vector>
#include "model.h"
#include <glm/gtx/quaternion.hpp>

// a class for building and rendering cubes
class SpheresScene {

	// Program
	
	GLuint instanceCount;
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Model> sphere;

	const float dist{ 0.14f };
	const int GRID_SIZE{ 5 };

public:
	std::vector<glm::mat4> instance_positions;
	int curBall = -1;
	const float scale{ 0.07f };
	glm::mat4 T;
	glm::mat4 C;

	SpheresScene();

	void render(const glm::mat4& projection, const glm::mat4& view);
	glm::vec3 getBallPos(int index);
	bool withinBound(glm::vec3 pt);
};