#pragma once
#include "RiftApp.h"
#include "ColorCubeScene.h"

// An example application that renders a simple cube
class ExampleApp : public RiftApp {
	std::shared_ptr<ColorCubeScene> cubeScene;

public:
	ExampleApp();

protected:
	void initGl() override;

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;
};