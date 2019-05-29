#pragma once
#include <glm/mat4x2.hpp>
#include "Cube.h"
#include <memory>
#include "shader.h"
#include <vector>
#include "model.h"
#include "TexturedCube.h"
#include "Skybox.h"

enum RenderingMode {
	RENDER_ENTIRESCENE,
	RENDER_STEREO,
	RENDER_MONO
};

enum EYE {
	LEFT_EYE,
	RIGHT_EYE
};

// a class for building and rendering cubes
class ColorCubeScene {

	// Program
	std::vector<glm::mat4> instance_positions;
	GLuint instanceCount;
	std::unique_ptr<Shader> skyboxShader;
	std::unique_ptr<TexturedCube> cube;
	std::unique_ptr<Skybox> skyboxR;
	std::unique_ptr<Skybox> skyboxL;

public:
	int renderingMode;
	int currentEye;
	float cubeScale = 30.0f;

	ColorCubeScene();

	void render(const glm::mat4& projection, const glm::mat4& view);
};