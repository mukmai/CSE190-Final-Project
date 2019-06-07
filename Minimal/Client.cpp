#include "Client.h"
#include <ctime>
#include "OVRInputWrapper.h"
#include "EntityManager.h"

Client::Client() : c("localhost", PORT)
{
	std::cout << "Connected to port: " << PORT << std::endl;
	playerController.playerID = c.call(serverFunction[PLAYER_JOIN]).as<int>();

	//leftPS->init();
	//rightPS->init();

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
	leftPS = new AParticleSystem();
	rightPS = new AParticleSystem();
	std::cout << "***** Finished initializing particle systems" << std::endl;
}

void Client::initAudio() {
	// NOTE: All audio is set up in the ASound constructor.
	//       This function is really just to autoplay the BGM.
	soundBGM = new ASound("Resources/Audio/BGM/bgm1.mp3");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/bgm2.mp3");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/bgm3.wav");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/bgm4.mp3");
	bgmList.push_back(soundBGM);
	soundBGM = new ASound("Resources/Audio/BGM/bgm5.wav");
	bgmList.push_back(soundBGM);

	soundBGM = bgmList[0];
	soundBGM->playSound();

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

	// XXX: QUICK HACK FOR PARTICLE SYSTEMS
	playerController.leftHandPos = controllerPosition[0];
	playerController.rightHandPos = controllerPosition[1];

	// Particle Systems
	//std::cout << "**** Attempting to update particle systems" << std::endl;
	leftPS->update(playerController.leftHandPos, glm::toMat4(playerController.leftHandRotation), eyePos);
	rightPS->update(playerController.rightHandPos, glm::toMat4(playerController.leftHandRotation), eyePos);

	//std::cout << "**** Attempting to render particle systems" << std::endl;
	leftPS->render(projection, glm::inverse(globalHeadPose));
	rightPS->render(projection, glm::inverse(globalHeadPose));
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

	bool bSpawnProjectileRight = OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right);
	if (bSpawnProjectileRight) {
		std::cout << "Right trigger pressed!" << std::endl;

		// STEP: Play fire sound
		soundFire->playSound();

		//// GOAL: Create a new Projectile Entity at the right hand
		//BaseState proj;

		//proj.pos = playerController.rightHandPos;
		//proj.rotation = playerController.rightHandRotation;
		//proj.scale = glm::vec3(1.0f);

		//proj.type = ENTITY_PROJECTILE;

		//proj.id = 10;

		//// STEP: Add this entity to the entity manager's list of entities
		//EntityManager::getInstance().getEntity(proj);

		//c.call(serverFunction[PROJECTILE_MOVE], proj.id, 0.1f);
	}

	bool bPlayBGM = OVRInputWrapper::getInstance().buttonPressed(ovrButton_A);
	if (bPlayBGM) {
		std::cout << "A button pressed!" << std::endl;
	}


	// send pos of all things and update all states
	vector<BaseState> newStates = c.call(serverFunction[GET_UPDATE], playerController.playerID).as<vector<BaseState>>();
	//std::cout << "Updating: ";
	for (int i = 0; i < newStates.size(); i++) {
		EntityManager::getInstance().update(newStates[i]);
		//std::cout << newStates[i].id << " ";
	}
	//std::cout << std::endl;
}