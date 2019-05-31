#include "SpheresScene.h"

SpheresScene::SpheresScene()
{
	{
		for (int z = 0; z < GRID_SIZE; ++z)
		{
			for (int y = 0; y < GRID_SIZE; ++y)
			{
				for (int x = 0; x < GRID_SIZE; ++x)
				{
					float xpos = (x - (GRID_SIZE / 2)) * dist;
					float ypos = (y - (GRID_SIZE / 2)) * dist;
					float zpos = (z - (GRID_SIZE / 2)) * dist;
					glm::vec3 relativePosition = glm::vec3(xpos, ypos, zpos);
					instance_positions.push_back(glm::translate(glm::mat4(1.0f), relativePosition));
				}
			}
		}
	}

	instanceCount = instance_positions.size();

	// Shader Program 
	shader = std::make_unique<Shader>("./Resources/Shaders/shader.vert", "./Resources/Shaders/shader.frag");

	sphere = std::make_unique<Model>("./Resources/Models/sphere.obj", false);

}

void SpheresScene::render(const glm::mat4& projection, const glm::mat4& view)
{
	shader->use();
	shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader->setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
	shader->setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
	shader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	shader->setMat4("projection", projection);
	glm::mat4 M = T * C;
	for (int i = 0; i < instanceCount; i++)
	{
		if (i == curBall)
			shader->setInt("mode", 1);
		else
			shader->setInt("mode", 0);
		glm::mat4 model = M * instance_positions[i] * glm::scale(glm::mat4(1.0f), glm::vec3(scale));

		shader->setMat4("model", model);
		shader->setMat4("view", view);
		sphere->draw(*shader);
	}
}

glm::vec3 SpheresScene::getBallPos(int index)
{
	return T * C * instance_positions[index][3];
}

bool SpheresScene::withinBound(glm::vec3 pt)
{
	glm::mat4 M = T * C;
	glm::vec3 localPt = pt - glm::vec3(M[3]);
	glm::mat4 R = glm::translate(M, -glm::vec3(M[3]));
	localPt = glm::transpose(R) * glm::vec4(localPt,1);
	float maxX = GRID_SIZE / 2 * dist + scale / 2;
	return (localPt.x > -maxX && localPt.x < maxX &&
		localPt.y > -maxX && localPt.y < maxX &&
		localPt.z > -maxX && localPt.z < maxX);
}