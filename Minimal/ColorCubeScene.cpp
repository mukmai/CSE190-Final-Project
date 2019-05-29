#include "ColorCubeScene.h"

ColorCubeScene::ColorCubeScene()
{
	instance_positions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.3)));
	instance_positions.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.9)));

	instanceCount = instance_positions.size();

	// Shader Program 
	skyboxShader = std::make_unique<Shader>("./Shaders/skybox.vert", "./Shaders/skybox.frag");

	cube = std::make_unique<TexturedCube>("Textures/cube");

	// 10m wide sky box: size doesn't matter though
	skyboxL = std::make_unique<Skybox>("Textures/skybox_left");
	skyboxL->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));

	// 10m wide sky box: size doesn't matter though
	skyboxR = std::make_unique<Skybox>("Textures/skybox_right");
	skyboxR->toWorld = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));

	renderingMode = RENDER_ENTIRESCENE;
}

void ColorCubeScene::render(const glm::mat4& projection, const glm::mat4& view)
{
	switch (renderingMode) {
	case RENDER_ENTIRESCENE:
		for (int i = 0; i < instanceCount; i++)
		{
			// Scale to 20cm: 200cm * 0.1
			cube->toWorld = instance_positions[i] * glm::scale(glm::mat4(1.0f), glm::vec3(cubeScale / 200.0f));
			cube->draw(skyboxShader->ID, projection, view);
		}
		if (currentEye == LEFT_EYE) {
			skyboxL->draw(skyboxShader->ID, projection, view);
		} else if (currentEye == RIGHT_EYE) {
			skyboxR->draw(skyboxShader->ID, projection, view);
		}
		break;
	case RENDER_STEREO:
		if (currentEye == LEFT_EYE) {
			skyboxL->draw(skyboxShader->ID, projection, view);
		}
		else if (currentEye == RIGHT_EYE) {
			skyboxR->draw(skyboxShader->ID, projection, view);
		}
		break;
	case RENDER_MONO:
		skyboxL->draw(skyboxShader->ID, projection, view);
		break;
	}
}
