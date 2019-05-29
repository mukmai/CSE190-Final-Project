#pragma once
#include "RiftApp.h"
#include "ColorCubeScene.h"
#include "ControllerSphereModule.h"
#include "OVRInputWrapper.h"

enum ViewingMode {
	STEREO_VISION,
	MONO,
	LEFT_EYE_ONLY,
	RIGHT_EYE_ONLY,
	INVERTED_STEREO
};

enum HeadTrackingMode {
	REGULAR_TRACKING,
	ORIENTATION_ONLY,
	POSITION_ONLY,
	NO_TRACKING
};

const int BUFFERSIZE = 30;

class Proj2 : public RiftApp {
	std::shared_ptr<ColorCubeScene> colorCubeScene;
	std::shared_ptr<ControllerSphereModule> controllerScene;
	int viewingMode;
	int headTrackingMode;
	float IOD;
	float defaultEyeOffset[2];
	glm::mat4 cameraBuffer[BUFFERSIZE][2];
	glm::vec3 controllerBuffer[BUFFERSIZE][2];
	int currBufferPos;
	int trackingDelay;
	int renderingDelay;
	int currRDelayPos;
	ovrPosef trackedEyePoses[2];
	int renderingBufferPos;
	bool superRotation;
	glm::vec3 eyeMidPoint;
	glm::vec3 rightEyeOffset;

public:
	Proj2();

protected:
	void initGl() override;

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;

	void setEyeTracking() override;
};