#include "Client.h"
#include <ctime>
#include "OVRInputWrapper.h"
#include "EntityManager.h"

#include <chrono>

Client::Client() : c("localhost", PORT)
{
	std::cout << "Connected to port: " << PORT << std::endl;
	playerController.playerID = c.call(serverFunction[PLAYER_JOIN]).as<int>();
	initAudio();
	EntityManager::getInstance().setPlayerID(playerController.playerID);

	glm::mat4 tmpLeftMat = glm::toMat4(controllerRotation[0]);
	tmpLeftMat[3] = glm::vec4(controllerPosition[0], 0.0f);

	glm::mat4 tmprightMat = glm::toMat4(controllerRotation[0]);
	tmprightMat[3] = glm::vec4(controllerPosition[0], 0.0f);

	lastLeftHand = tmpLeftMat;
	lastRightHand = tmprightMat;

	leftCooldown = std::chrono::system_clock::now();
	rightCooldown = std::chrono::system_clock::now();
}

void Client::initGl()
{
	RiftApp::initGl();
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	//srand((unsigned int)time(NULL));
	unsigned int milliseconds_since_epoch =
		std::chrono::system_clock::now().time_since_epoch() /
		std::chrono::milliseconds(1);
	srand(milliseconds_since_epoch);
	sphereScene = std::shared_ptr<SpheresScene>(new SpheresScene());
}

void Client::initAudio() {
	// NOTE: All audio is set up in the ASound constructor.
	//       This function is really just to autoplay the BGM.
	soundBGM = new ASound("Resources/Audio/BGM/Moment of Impact V2.wav");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/Conciliation.wav");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/Riverside.wav");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/Fireworks Over Barcelona.wav");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/Wayang Kulit.wav");
	bgmList.push_back(soundBGM);

	checkBGM();

	std::cout << "BGM should be playing..." << std::endl;
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
	auto globalEyePose = globalPlayerTranslation * globalPlayerRotation * glm::vec4(eyePos, 1);
	EntityManager::getInstance().render(projection, glm::inverse(globalHeadPose), globalEyePose);
}

void Client::update()
{
	RiftApp::update();

	//oldTime = newTime;
	//newTime = ovr_GetTimeInSeconds();
	std::chrono::system_clock::time_point time_now = std::chrono::system_clock::now();

	// send player head, controllers position and rotation to server
	playerController.leftHandPos = controllerPosition[0];
	playerController.leftHandRotation = controllerRotation[0];

	playerController.rightHandPos = controllerPosition[1];
	playerController.rightHandRotation = controllerRotation[1];

	// Needed for checking the rotation of the player's hand
	glm::mat4 tmpLeftMat = glm::toMat4(controllerRotation[0]);
	tmpLeftMat[3] = glm::vec4(controllerPosition[0], 0.0f);

	glm::mat4 tmpRightMat = glm::toMat4(controllerRotation[1]);
	tmpRightMat[3] = glm::vec4(controllerPosition[1], 0.0f);

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
		
	// Check if the left hand rotated enough
	glm::vec3 leftA = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 leftB = glm::vec3(0.0f, 1.0f, 0.0f);

	leftA = lastLeftHand * glm::vec4(leftA, 0.0f);
	leftB = tmpLeftMat * glm::vec4(leftB, 0.0f);

	glm::vec3 crossLeft = glm::cross(leftA, leftB);
	float leftAngY = glm::length(crossLeft);

	// Check if the left hand rotated enough
	leftA = glm::vec3(1.0f, 0.0f, 0.0f);
	leftB = glm::vec3(1.0f, 0.0f, 0.0f);

	leftA = lastLeftHand * glm::vec4(leftA, 0.0f);
	leftB = tmpLeftMat * glm::vec4(leftB, 0.0f);

	crossLeft = glm::cross(leftA, leftB);
	float leftAngX = glm::length(crossLeft);

	//if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_Y)) {
	if(leftAngY > 0.4f && leftAngX > 0.4f && (std::chrono::duration_cast<std::chrono::milliseconds>(time_now - leftCooldown).count() > 500)){
		c.call(serverFunction[PLAYER_LEFT_SWITCH], playerController.playerID);
		leftCooldown = std::chrono::system_clock::now();
	}

	// Check if the right hand rotated enough
	glm::vec3 rightA = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rightB = glm::vec3(0.0f, 1.0f, 0.0f);

	rightA = lastRightHand * glm::vec4(rightA, 0.0f);
	rightB = tmpRightMat * glm::vec4(rightB, 0.0f);

	glm::vec3 crossRight = glm::cross(rightA, rightB);
	float rightAngY = glm::length(crossRight);

	// Check if the right hand rotated enough
	rightA = glm::vec3(1.0f, 0.0f, 0.0f);
	rightB = glm::vec3(1.0f, 0.0f, 0.0f);

	rightA = lastRightHand * glm::vec4(rightA, 0.0f);
	rightB = tmpRightMat * glm::vec4(rightB, 0.0f);

	crossRight = glm::cross(rightA, rightB);
	float rightAngX = glm::length(crossRight);

	//if (OVRInputWrapper::getInstance().buttonPressed(ovrButton_B)) {
	if (rightAngY > 0.4f && rightAngX > 0.4f && (std::chrono::duration_cast<std::chrono::milliseconds>(time_now - rightCooldown).count() > 500)) {
		c.call(serverFunction[PLAYER_RIGHT_SWITCH], playerController.playerID);
		rightCooldown = std::chrono::system_clock::now();
	}

	float leftGripTriggerRate = OVRInputWrapper::getInstance().gripTriggerRate(ovrHand_Left);
	if (leftGripTriggerRate > 0.15f) {
		c.call(serverFunction[PLAYER_LEFT_THRUSTER], playerController.playerID, leftGripTriggerRate);
	}
	else {
		c.call(serverFunction[PLAYER_LEFT_THRUSTER_OFF], playerController.playerID);
	}

	float rightGripTriggerRate = OVRInputWrapper::getInstance().gripTriggerRate(ovrHand_Right);
	if (rightGripTriggerRate > 0.15f) {
		c.call(serverFunction[PLAYER_RIGHT_THRUSTER], playerController.playerID, rightGripTriggerRate);
	}
	else {
		c.call(serverFunction[PLAYER_RIGHT_THRUSTER_OFF], playerController.playerID);
	}

	bool bLeftTriggerPressed = OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Left);
	if (bLeftTriggerPressed) {
		// STEP: Play fire sound
		soundFire->playSound3DInstance(1.0f);

		c.call(serverFunction[PLAYER_SHOOT], playerController.playerID, 0);
	}

	bool bRightTriggerPressed = OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right);
	if (bRightTriggerPressed) {
		// STEP: Play fire sound
		soundFire->playSound3DInstance(1.0f);

		c.call(serverFunction[PLAYER_SHOOT], playerController.playerID, 1);
	}

	// send pos of all things and update all states
	vector<BaseState> newStates = c.call(serverFunction[GET_UPDATE], playerController.playerID).as<vector<BaseState>>();
	for (int i = 0; i < newStates.size(); i++) {
		EntityManager::getInstance().update(newStates[i]);
	}
	//std::cout << std::endl;

	lastLeftHand = tmpLeftMat;
	lastRightHand = tmpRightMat;

	// STEP: Check that the BGM is playing properly
	checkBGM();
}

void Client::checkBGM() {
	// Check if the BGM has stopped playing.
	// If it has, play a random song from the BGM list.

	// Check that the soundBGM ptr is NOT NULL
	if (soundBGM != NULL) {
		// If the ptr is not NULL, check that its audio channel is playing
		bool bTempPlaying;
		soundBGM->channel->isPlaying(&bTempPlaying);
		if (!bTempPlaying) {
			// Just in case, manually stop the channel
			soundBGM->stopSound();

			// Pick a random index for the BGM list
			unsigned int idx = rand() % (bgmList.size());

			// Reassign soundBGM to a randomly selected BGM
			soundBGM = bgmList[idx];
			soundBGM->playSound(BGM_VOLUME);
		}
	}
	else {
		// Pick a random index for the BGM list
		unsigned int idx = rand() % (bgmList.size());

		// Reassign soundBGM to a randomly selected BGM
		soundBGM = bgmList[idx];
		soundBGM->playSound(BGM_VOLUME);
	}
}