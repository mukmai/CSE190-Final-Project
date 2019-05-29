#include "ColorCubeScene2.h"

ColorCubeScene2::ColorCubeScene2()
{

	// Shader Program 
	skyboxShader = std::make_unique<Shader>("./Shaders/skybox.vert", "./Shaders/skybox.frag");

	cube = std::make_unique<TexturedCube>("Textures/cube");

	// 10m wide sky box: size doesn't matter though
	skyboxL = std::make_unique<Skybox>("Textures/skybox_left");
	skyboxL->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));

	// 10m wide sky box: size doesn't matter though
	skyboxR = std::make_unique<Skybox>("Textures/skybox_right");
	skyboxR->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));

	worldPos = glm::vec3(0, 0, -0.3f);
}

void ColorCubeScene2::render(const glm::mat4& projection, const glm::mat4& view)
{
	glm::mat4 wT = glm::translate(glm::mat4(1.0f), worldPos);
	// Scale to 20cm: 200cm * 0.1
	cube->toWorld = wT * glm::scale(glm::mat4(1.0f), glm::vec3(cubeScale / 200.0f));
	cube->draw(skyboxShader->ID, projection, view);

	if (currentEye == LEFT_EYE) {
		skyboxL->draw(skyboxShader->ID, projection, view);
	}
	else if (currentEye == RIGHT_EYE) {
		skyboxR->draw(skyboxShader->ID, projection, view);
	}
}
