#include "Proj2.h"

Proj2::Proj2()
{
}

void Proj2::initGl()
{
	RiftApp::initGl();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	colorCubeScene = std::shared_ptr<ColorCubeScene>(new ColorCubeScene());
	controllerScene = std::shared_ptr<ControllerSphereModule>(new ControllerSphereModule());
	viewingMode = STEREO_VISION;
	headTrackingMode = REGULAR_TRACKING;
	currBufferPos = 0;
	trackingDelay = 0;
	renderingDelay = 0;
	currRDelayPos = 0;
	superRotation = false;

	defaultEyeOffset[ovrEye_Left] = _viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x;
	defaultEyeOffset[ovrEye_Right] = _viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x;
	IOD = _viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x - _viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x;
	printf("left: %f, right: %f, IOD: %f\n", _viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x, _viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x, IOD);
}

void Proj2::shutdownGl()
{
	colorCubeScene.reset();
	controllerScene.reset();
}

void Proj2::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	colorCubeScene->currentEye = renderingEye;

	switch (viewingMode) {
	case STEREO_VISION:
		controllerScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
		colorCubeScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
		break;
	case MONO:
		controllerScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][ovrEye_Left]));
		colorCubeScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][ovrEye_Left]));
		break;
	case LEFT_EYE_ONLY:
		if (renderingEye == ovrEye_Left) {
			controllerScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
			colorCubeScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
		}
		break;
	case RIGHT_EYE_ONLY:
		if (renderingEye == ovrEye_Right) {
			controllerScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
			colorCubeScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][renderingEye]));
		}
		break;
	case INVERTED_STEREO:
		int temp_eye = (renderingEye + 1) % 2;
		controllerScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][temp_eye]));
		colorCubeScene->render(projection, glm::inverse(cameraBuffer[renderingBufferPos][temp_eye]));
		break;
	}
}

void Proj2::setEyeTracking()
{
	glm::vec3 newEyeMidPoint;
	glm::vec3 midPointDiff;

	switch (headTrackingMode) {
	case REGULAR_TRACKING:

		eyeMidPoint = (ovr::toGlm(eyePoses[0].Position) + ovr::toGlm(eyePoses[1].Position)) / 2.0f;
		rightEyeOffset = (ovr::toGlm(eyePoses[1].Position) - eyeMidPoint);

		ovr::for_each_eye([&](ovrEyeType eye)
		{
			trackedEyePoses[eye] = eyePoses[eye];
		});
		break;
	case ORIENTATION_ONLY:

		newEyeMidPoint = (ovr::toGlm(eyePoses[0].Position) + ovr::toGlm(eyePoses[1].Position)) / 2.0f;
		midPointDiff = eyeMidPoint - newEyeMidPoint;
		rightEyeOffset = (ovr::toGlm(eyePoses[1].Position) - newEyeMidPoint);

		ovr::for_each_eye([&](ovrEyeType eye)
		{
			trackedEyePoses[eye].Position = ovr::fromGlm(ovr::toGlm(eyePoses[eye].Position) + midPointDiff);
			trackedEyePoses[eye].Orientation = eyePoses[eye].Orientation;
		});
		break;
	case POSITION_ONLY:

		eyeMidPoint = (ovr::toGlm(eyePoses[0].Position) + ovr::toGlm(eyePoses[1].Position)) / 2.0f;

		ovr::for_each_eye([&](ovrEyeType eye)
		{
			glm::vec3 newEyePos = eyeMidPoint + ((eye) ? rightEyeOffset : -rightEyeOffset);
			trackedEyePoses[eye].Position = ovr::fromGlm(newEyePos);
		});
		break;
	}
	glm::mat4 leftPos = ovr::toGlm(trackedEyePoses[ovrEye_Left]);
	glm::mat4 rightPos = ovr::toGlm(trackedEyePoses[ovrEye_Right]);
	if (superRotation && headTrackingMode != POSITION_ONLY) {
		for (int i = 0; i < 2; i++)
		{
			glm::quat q = ovr::toGlm(trackedEyePoses[i].Orientation);
			glm::quat forward = glm::quat(0, 0, 0, -1);
			forward = q * forward * glm::conjugate(q);
			glm::vec3 angles = glm::eulerAngles(q);

			float a = acos(glm::dot(glm::vec2(0, -1), glm::normalize(glm::vec2(forward.x, forward.z))));
			a = copysignf(a, angles.y) * 2;
			glm::quat temp2 = glm::angleAxis(a - angles.y, glm::vec3(0, 1, 0));
			leftPos = glm::mat4_cast(temp2) * leftPos;
			rightPos = glm::mat4_cast(temp2) * rightPos;
		}
	}

	cameraBuffer[currBufferPos][ovrEye_Left] = leftPos;
	cameraBuffer[currBufferPos][ovrEye_Right] = rightPos;
}


void Proj2::update()
{
	RiftApp::update();

	// change render mode for scene
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_X)) {
		colorCubeScene->renderingMode = (colorCubeScene->renderingMode + 1) % 3;
	}
	// increase size of cube
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Left) > 0) {
		colorCubeScene->cubeScale = min(50.0f, colorCubeScene->cubeScale + 1.0f);
	}
	// decrease size of cube
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Left) < 0) {
		colorCubeScene->cubeScale = max(1.0f, colorCubeScene->cubeScale - 1.0f);
	}
	// reset size of cube
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_LThumb)) {
		colorCubeScene->cubeScale = 30.0f;
	}
	// change render mode
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_A)) {
		viewingMode = (viewingMode + 1) % 5;
		printf("viewingMode: %d\n", viewingMode);
	}
	// change head tracking mode
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_B)) {
		headTrackingMode = (headTrackingMode + 1) % 4;
		printf("headTracking: %d\n", headTrackingMode);
	}
	// increase IOD
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Right) > 0) {
		float rate = min(0.3f - IOD, 0.005f) / 2;
		_viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x -= rate;
		_viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x += rate;
		IOD += rate * 2;
	}
	// decrease IOD
	if (OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Right) < 0) {
		float rate = min(0.1f + IOD, 0.005f) / 2;
		_viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x += rate;
		_viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x -= rate;
		IOD -= rate * 2;
	}
	// reset IOD
	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_RThumb)) {
		_viewScaleDesc.HmdToEyePose[ovrEye_Left].Position.x = defaultEyeOffset[ovrEye_Left];
		_viewScaleDesc.HmdToEyePose[ovrEye_Right].Position.x = defaultEyeOffset[ovrEye_Right];
		IOD = defaultEyeOffset[ovrEye_Right] - defaultEyeOffset[ovrEye_Left];
	}
	
	if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right)) {
		if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_Y)) {
			// increase smoothing mode n
			printf("Smoothing controller: %d frames\n", controllerScene->incSmooth());
		}
		else {
			// increase tracking lag
			trackingDelay = min(trackingDelay + 1, BUFFERSIZE - 1);
			printf("Tracking lag: %d frames\n", trackingDelay);
		}
	}
	
	if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Left)) {
		if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_Y)) {
			// decrease smoothing mode n
			printf("Smoothing controller: %d frames\n", controllerScene->decSmooth());
		}
		else {
			// decrease tracking lag
			trackingDelay = max(trackingDelay - 1, 0);
			printf("Tracking lag: %d frames\n", trackingDelay);
		}
	}

	if (OVRInputWrapper::getInstance().gripTriggerPressed(ovrHand_Right)) {
		if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_Y)) {
			// turn on super-rotation
			superRotation = true;
		}
		else {
			// increase rendering lag
			renderingDelay = min(renderingDelay + 1, 10);
			printf("Rendering delay: %d frames\n", renderingDelay);
		}
	}

	if (OVRInputWrapper::getInstance().gripTriggerPressed(ovrHand_Left)) {

		if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_Y)) {
			// turn off super-rotation
			superRotation = false;
		}
		else {
			// decrease rendering lag
			renderingDelay = max(renderingDelay - 1, 0);
			printf("Rendering delay: %d frames\n", renderingDelay);
		}
	}

	// update render delay offset
	if (renderingDelay == 0) {
		currRDelayPos = 0;
	}
	else {
		currRDelayPos = (currRDelayPos + 1) % renderingDelay;
	}

	currBufferPos = (currBufferPos + 1) % BUFFERSIZE;

	controllerBuffer[currBufferPos][ovrHand_Left] = controllerPosition[ovrHand_Left];
	controllerBuffer[currBufferPos][ovrHand_Right] = controllerPosition[ovrHand_Right];
	

	renderingBufferPos = currBufferPos - trackingDelay - currRDelayPos + BUFFERSIZE * 2;
	renderingBufferPos %= BUFFERSIZE;

	controllerScene->update(controllerBuffer[renderingBufferPos]);
}

