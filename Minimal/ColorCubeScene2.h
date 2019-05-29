#pragma once
#include <glm/mat4x2.hpp>
#include "Cube.h"
#include <memory>
#include "shader.h"
#include <vector>
#include "model.h"
#include "TexturedCube.h"
#include "Skybox.h"
#include "Renderable.h"

enum EYE {
	LEFT_EYE,
	RIGHT_EYE
};

// a class for building and rendering cubes
class ColorCubeScene2 : public Renderable {

	// Program
	std::unique_ptr<Shader> skyboxShader;
	std::unique_ptr<TexturedCube> cube;
	std::unique_ptr<Skybox> skyboxR;
	std::unique_ptr<Skybox> skyboxL;

public:
	int currentEye;
	float cubeScale = 30.0f;
	glm::vec3 worldPos;

	ColorCubeScene2();

	void render(const glm::mat4& projection, const glm::mat4& view) override;
};