#pragma once
#include "rpc/client.h"
#include "RiftApp.h"
#include "SpheresScene.h"
#include "ControllerSphereModule.h"

#include "BaseState.h"
#include "Common.h"

// An example application that renders a simple cube
class Client : public RiftApp {
	std::shared_ptr<SpheresScene> sphereScene;
	std::shared_ptr<ControllerSphereModule> controllerScene;
	double oldTime = 0;
	double newTime = 0;
	BaseState exampleState;
	rpc::client c;
	int playerID;

public:
	Client();

protected:
	void initGl() override;

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;
};