#include "Client.h"
#include <ctime>
#include "OVRInputWrapper.h"
#include "EntityManager.h"

Client::Client() : c("localhost", PORT)
{
	std::cout << "Connected to port: " << PORT << std::endl;
	playerController.playerID = c.call(serverFunction[PLAYER_JOIN]).as<int>();
	EntityManager::getInstance().setPlayerID(playerController.playerID);
}

void Client::initGl()
{
	RiftApp::initGl();
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	srand((unsigned int)time(NULL));
	sphereScene = std::shared_ptr<SpheresScene>(new SpheresScene());

}

void Client::shutdownGl()
{
}

void Client::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	//sphereScene->render(projection, glm::inverse(headPose));
	auto eyePos = ovr::toGlm(eyePoses[renderingEye].Position);
	auto playerState = EntityManager::getInstance().getEntity(playerController.playerID)->getState();
	auto globalPlayerTranslation = glm::translate(glm::mat4(1), playerState->pos);
	auto globalPlayerRotation = glm::mat4_cast(playerState->rotation);
	auto globalHeadPose = globalPlayerTranslation * globalPlayerRotation * headPose;
	EntityManager::getInstance().render(projection, glm::inverse(globalHeadPose), eyePos);
}

void Client::update()
{
	RiftApp::update();

	//oldTime = newTime;
	//newTime = ovr_GetTimeInSeconds();

	// send player head, controllers position and rotation to server
	playerController.leftHandPos = controllerPosition[0];
	playerController.leftHandRotation = controllerRotation[0];

	playerController.rightHandPos = controllerPosition[1];
	playerController.rightHandRotation = controllerRotation[1];

	glm::quat headRotation = ovr::toGlm(eyePoses[0].Orientation);
	glm::vec3 headPos = (ovr::toGlm(eyePoses[0].Position) + ovr::toGlm(eyePoses[1].Position)) / 2.0f;
	headPos += headRotation * glm::vec3(0, 0, 1.0f) * 0.065f;
	playerController.headPos = headPos;
	playerController.headRotation = headRotation;

	c.call(serverFunction[PLAYER_INPUT], playerController);

	float leftThumbStickVertical = OVRInputWrapper::getInstance().thumbStickVertical(ovrHand_Left);
	if (leftThumbStickVertical != 0) {
		c.call(serverFunction[PLAYER_MOVE_Z], playerController.playerID, leftThumbStickVertical);
	}

	float leftThumbStickHorizontal = OVRInputWrapper::getInstance().thumbStickHorizontal(ovrHand_Left);
	if (leftThumbStickHorizontal != 0) {
		c.call(serverFunction[PLAYER_MOVE_X], playerController.playerID, leftThumbStickHorizontal);
	}

	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_A)) {
		c.call(serverFunction[PLAYER_BUTTON_A], playerController.playerID);
	}

	if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_X)) {
		c.call(serverFunction[PLAYER_BUTTON_X], playerController.playerID);
	}

	//if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_Y)) {
	//	c.call(serverFunction[PLAYER_BUTTON_Y], playerController.playerID);
	//}

	//if (OVRInputWrapper::getInstance().buttonHeld(ovrButton_B)) {
	//	c.call(serverFunction[PLAYER_BUTTON_B], playerController.playerID);
	//}

	float leftGripTriggerRate = OVRInputWrapper::getInstance().gripTriggerRate(ovrHand_Left);
	if (leftGripTriggerRate > 0.15f) {
		c.call(serverFunction[PLAYER_LEFT_THRUSTER], playerController.playerID, leftGripTriggerRate);
	}

	float rightGripTriggerRate = OVRInputWrapper::getInstance().gripTriggerRate(ovrHand_Right);
	if (leftGripTriggerRate > 0.15f) {
		c.call(serverFunction[PLAYER_RIGHT_THRUSTER], playerController.playerID, rightGripTriggerRate);
	}

	// send pos of all things and update all states
	vector<BaseState> newStates = c.call(serverFunction[GET_UPDATE], playerController.playerID).as<vector<BaseState>>();
	for (int i = 0; i < newStates.size(); i++) {
		EntityManager::getInstance().update(newStates[i]);
	}
}