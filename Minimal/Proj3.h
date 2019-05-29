#pragma once
#pragma once
#include "RiftApp.h"
#include "Renderable.h"
#include "ColorCubeScene2.h"
#include "ControllerSphereModule.h"
#include "OVRInputWrapper.h"
#include <vector>
#include "CaveComponent.h"

class Proj3 : public RiftApp {
	std::shared_ptr<ColorCubeScene2> colorCubeScene;
	std::shared_ptr<ControllerSphereModule> controllerSphere;
	std::shared_ptr<CaveComponent> caveComponent_bot;
	std::shared_ptr<CaveComponent> caveComponent_left;
	std::shared_ptr<CaveComponent> caveComponent_right;
	float defaultEyeOffset[2];
	ovrPosef trackedEyePoses[2];
	vector<std::shared_ptr<Renderable>> renderList;
	vector<std::shared_ptr<CaveComponent>> cave;
	float height;
	float caveWidth;
	glm::vec3 caveRotation;
	bool head_in_hand;
	bool freezeTracking;
	float IOD;
	float speed;
	int failedProjector;
	bool projectorFailure;

public:
	Proj3();

protected:
	void initGl() override;

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;

	void setEyeTracking() override;
};