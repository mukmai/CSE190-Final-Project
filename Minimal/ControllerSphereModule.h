#pragma once
#include <glm/mat4x2.hpp>
#include <memory>
#include "shader.h"
#include <vector>
#include "model.h"
#include "RiftApp.h"
#include "Renderable.h"
#include <queue>

class ControllerSphereModule : public Renderable
{
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Model> sphere;
	glm::mat4 sphereScaleM;
	glm::vec3 totalPos[2];
	std::queue<glm::vec3> posBuffer[2];
	const int MAX_SMOOTH = 45;
	glm::vec3 movingAverage[2];

public:
	const float scale{ 0.02f };
	
	int smoothN;

	ControllerSphereModule();

	void render(const glm::mat4& projection, const glm::mat4& view) override;

	int incSmooth();

	int decSmooth();

	void update(glm::vec3 controllerPosition[2]);
};

