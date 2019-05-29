#pragma once
#include "RiftApp.h"
#include "SpheresScene.h"
#include "ControllerSphereModule.h"

// An example application that renders a simple cube
class Proj1 : public RiftApp {
	std::shared_ptr<SpheresScene> sphereScene;
	std::shared_ptr<ControllerSphereModule> controllerScene;
	bool gameStart = false;
	unsigned int score = 0;
	double oldTime = 0;
	double newTime = 0;
	double timer = 0;
	glm::quat origQuat;
	glm::vec3 origPos;
	glm::mat4 origT;
	bool grabbingSphere = false;
	
	const int BEGIN_TIME{ 60 };

public:
	Proj1();

protected:
	void initGl() override;

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;

	bool checkHit();
};