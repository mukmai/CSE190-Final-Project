#include "Client.h"
#include <ctime>
#include "OVRInputWrapper.h"
#include "EntityManager.h"

Client::Client() : c("localhost", PORT)
{
	std::cout << "Connected to port: " << PORT << std::endl;
	playerController.playerID = c.call(serverFunction[PLAYER_JOIN]).as<int>();
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
	EntityManager::getInstance().render(projection, glm::inverse(headPose), eyePos);
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
	// TODO: head position
	c.call(serverFunction[PLAYER_INPUT], playerController);

	// send pos of all things and update all states
	vector<BaseState> newStates = c.call(serverFunction[GET_UPDATE], playerController.playerID).as<vector<BaseState>>();
	//std::cout << "Updating: ";
	for (int i = 0; i < newStates.size(); i++) {
		EntityManager::getInstance().update(newStates[i]);
		//std::cout << newStates[i].id << " ";
	}
	//std::cout << std::endl;
}