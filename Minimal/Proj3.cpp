#include "Proj3.h"

Proj3::Proj3()
{
}

#define addToRenderList(entity) renderList.push_back(std::static_pointer_cast<Renderable>(entity));

void Proj3::initGl()
{
	height = 1.73f;
	caveWidth = 2.4f;
	caveRotation = glm::vec3(0.0f, -45.0f, 0.0f);
	IOD = 0.065f;
	head_in_hand = false;
	freezeTracking = false;
	speed = 0.005f;
	projectorFailure = false;

	RiftApp::initGl();
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);

	// bottom cave component
	caveComponent_bot = std::shared_ptr<CaveComponent>(
		new CaveComponent(
			glm::vec3(0.0f, -height, 0.0f),
			glm::vec3(-90.0f, 0.0f, 0.0f), 
			glm::vec3(caveWidth, caveWidth, caveWidth),
			caveRotation
		));
	addToRenderList(caveComponent_bot);
	cave.push_back(caveComponent_bot);

	// left cave component
	caveComponent_left = std::shared_ptr<CaveComponent>(
		new CaveComponent(
			glm::vec3(-caveWidth / 2, caveWidth / 2 - height, 0.0f),
			glm::vec3(0.0f, 90.0f, 0.0f),
			glm::vec3(caveWidth, caveWidth, caveWidth),
			caveRotation
		));
	addToRenderList(caveComponent_left);
	cave.push_back(caveComponent_left);

	// right cave component
	caveComponent_right = std::shared_ptr<CaveComponent>(
		new CaveComponent(
			glm::vec3(0.0f, caveWidth / 2 - height, - caveWidth / 2),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(caveWidth, caveWidth, caveWidth),
			caveRotation
		));
	addToRenderList(caveComponent_right);
	cave.push_back(caveComponent_right);

	colorCubeScene = std::shared_ptr<ColorCubeScene2>(new ColorCubeScene2());
	controllerSphere = std::shared_ptr<ControllerSphereModule>(new ControllerSphereModule());

	// List of rendering objects into cave
	for (auto &component : cave) {
		component->addRenderList(colorCubeScene);
		component->addRenderList(controllerSphere);
	}
	//addToRenderList(controllerSphere);

	defaultEyeOffset[ovrEye_Left] = _viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x;
	defaultEyeOffset[ovrEye_Right] = _viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x;
}

void Proj3::shutdownGl()
{
	colorCubeScene.reset();
	controllerSphere.reset();
	for (auto &component : cave) {
		component.reset();
	}
}

void Proj3::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	colorCubeScene->currentEye = renderingEye;
	CaveComponent::currentEye = renderingEye;
	for (auto &renderObj : renderList) {
		renderObj->render(projection, glm::inverse(headPose));
	}
}

void Proj3::setEyeTracking()
{
	if (!freezeTracking) {
		if (!head_in_hand) {
			CaveComponent::trackingEyePos[ovrEye_Left] = ovr::toGlm(eyePoses[ovrEye_Left].Position);
			CaveComponent::trackingEyePos[ovrEye_Right] = ovr::toGlm(eyePoses[ovrEye_Right].Position);
		}
		else {
			glm::vec3 leftEye = controllerPosition[ovrHand_Right] + controllerRotation[ovrHand_Right] * glm::vec3(-IOD / 2, 0, 0);
			glm::vec3 rightEye = controllerPosition[ovrHand_Right] + controllerRotation[ovrHand_Right] * glm::vec3(IOD / 2, 0, 0);
			CaveComponent::trackingEyePos[ovrEye_Left] = leftEye;
			CaveComponent::trackingEyePos[ovrEye_Right] = rightEye;
		}
	}
	CaveComponent::realEyePos[ovrEye_Left] = ovr::toGlm(eyePoses[ovrEye_Left].Position);
	CaveComponent::realEyePos[ovrEye_Right] = ovr::toGlm(eyePoses[ovrEye_Right].Position);
}


void Proj3::update()
{
	RiftApp::update();
	// increase size of cube
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Right) > 0.3f) {
		colorCubeScene->cubeScale = min(50.0f, colorCubeScene->cubeScale + 1.0f);
	}
	// decrease size of cube
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Right) < -0.3f) {
		colorCubeScene->cubeScale = max(1.0f, colorCubeScene->cubeScale - 1.0f);
	}
	// reset size of cube
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_LThumb)) {
		colorCubeScene->cubeScale = 30.0f;
	}

	// move cube to right
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Left) > 0.3f) {
		colorCubeScene->worldPos += glm::vec3(speed, 0, 0);
	}
	// move cube to left
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Left) < -0.3f) {
		colorCubeScene->worldPos -= glm::vec3(speed, 0, 0);
	}
	// move cube to front
	if (OVRInputWrapper::getInstance().thumbStickVertical(ovrHand_Left) > 0.3f) {
		colorCubeScene->worldPos -= glm::vec3(0, 0, speed);
	}
	// move cube to back
	if (OVRInputWrapper::getInstance().thumbStickVertical(ovrHand_Left) < -0.3f) {
		colorCubeScene->worldPos += glm::vec3(0, 0, speed);
	}
	// move cube to up
	if (OVRInputWrapper::getInstance().thumbStickVertical(ovrHand_Right) > 0.3f) {
		colorCubeScene->worldPos += glm::vec3(0, speed, 0);
	}
	// move cube to down
	if (OVRInputWrapper::getInstance().thumbStickVertical(ovrHand_Right) < -0.3f) {
		colorCubeScene->worldPos -= glm::vec3(0, speed, 0);
	}

	// turn on head-in-hand mode
	if (OVRInputWrapper::getInstance().indexTriggerHeld(ovrHand_Right)) {
		head_in_hand = true;
	}
	// turn off head-in-hand mode
	if (OVRInputWrapper::getInstance().indexTriggerReleased(ovrHand_Right)) {
		head_in_hand = false;
	}
	// turn on/off freeze mode on cave
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_B)) {
		freezeTracking = !freezeTracking;
	}

	// turn on debug mode
	if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_A)) {
		CaveComponent::debugMode = true;
	}
	// turn off debug mode
	if (OVRInputWrapper::getInstance().buttonReleased(ovrButton_A)) {
		CaveComponent::debugMode = false;
	}

	// turn on/off brightness falloff on LCD screen
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_Y)) {
		CaveComponent::fallOffLCD = !CaveComponent::fallOffLCD;
	}

	// turn on/off random projector failure
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_X)) {
		if (projectorFailure) {
			projectorFailure = false;
			cave[failedProjector / 2]->failedProjector = 2;
		}
		else {
			projectorFailure = true;
			failedProjector = rand() % 6;
			cave[failedProjector / 2]->failedProjector = failedProjector % 2;
		}
	}

	controllerSphere->update(controllerPosition);
}

