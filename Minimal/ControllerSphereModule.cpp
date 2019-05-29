#include "ControllerSphereModule.h"

ControllerSphereModule::ControllerSphereModule()
{
	shader = std::make_unique<Shader>("./Shaders/shader.vert", "./Shaders/shader.frag");
	sphere = std::make_unique<Model>("./Models/sphere.obj", false);
	sphereScaleM = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	smoothN = 1;
	totalPos[0] = glm::vec3(0);
	totalPos[1] = glm::vec3(0);
}

void ControllerSphereModule::render(const glm::mat4& projection, const glm::mat4& view)
{
	shader->use();
	shader->setMat4("projection", projection);
	shader->setInt("mode", 2);
	for (int i = 0; i < 2; i++)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), movingAverage[i]) * sphereScaleM;

		shader->setMat4("model", model);
		shader->setMat4("view", view);
		sphere->draw(*shader);
	}

}

int ControllerSphereModule::incSmooth()
{
	smoothN = min(smoothN + 1, MAX_SMOOTH);
	return smoothN;
}

int ControllerSphereModule::decSmooth()
{
	smoothN = max(smoothN - 1, 1);
	return smoothN;
}

void ControllerSphereModule::update(glm::vec3 controllerPosition[2])
{
	for (int i = 0; i < 2; i++) {
		totalPos[i] += controllerPosition[i];
		posBuffer[i].push(controllerPosition[i]);
		while (posBuffer[i].size() > (size_t)smoothN) {
			totalPos[i] -= posBuffer[i].front();
			posBuffer[i].pop();
		}
		movingAverage[i] = totalPos[i] / (float)smoothN;
	}
}