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

}

void Client::initGl()
{
	RiftApp::initGl();
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	srand((unsigned int)time(NULL));
	sphereScene = std::shared_ptr<SpheresScene>(new SpheresScene());
	
	std::cout << "***** Initializing particle systems" << std::endl;
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
		c.call(serverFunction[PLAYER_MOVE], playerController.playerID, leftThumbStickVertical);
	}

	bool bRightTriggerPressed = OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right);
	if (bRightTriggerPressed) {
		std::cout << "Right trigger pressed!" << std::endl;

		// STEP: Play fire sound
		soundFire->playSound(1.0f);
	}

	bool bLeftTriggerPressed = OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Left);
	if (bLeftTriggerPressed) {
		std::cout << "Left trigger pressed!" << std::endl;

		// STEP: Play fire sound
		soundFire->playSound(1.0f);
	}

	// send pos of all things and update all states
	vector<BaseState> newStates = c.call(serverFunction[GET_UPDATE], playerController.playerID).as<vector<BaseState>>();
	//std::cout << "Updating: ";
	for (int i = 0; i < newStates.size(); i++) {
		EntityManager::getInstance().update(newStates[i]);
		//std::cout << newStates[i].id << " ";
	}
	//std::cout << std::endl;

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