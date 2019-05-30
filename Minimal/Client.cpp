#include "Client.h"
#include <ctime>
#include "OVRInputWrapper.h"

Client::Client() : c("localhost", PORT)
{
	std::cout << "Connected to port: " << PORT << std::endl;
}

void Client::initGl()
{
	RiftApp::initGl();
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	sphereScene = std::shared_ptr<SpheresScene>(new SpheresScene());
	controllerScene = std::shared_ptr<ControllerSphereModule>(new ControllerSphereModule());
	srand(time(NULL));

	exampleState.hp = 100;
	exampleState.pos = glm::vec3(1.0f, 2.5f, -0.3f);
}

void Client::shutdownGl()
{
	sphereScene.reset();
	controllerScene.reset();
}

void Client::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	controllerScene->render(projection, glm::inverse(headPose));
	sphereScene->render(projection, glm::inverse(headPose));
}

void Client::update()
{
	RiftApp::update();

	oldTime = newTime;
	newTime = ovr_GetTimeInSeconds();

	if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right))
	{
		/* Call echo rpc function that is defined on the server */
		/* Need to cast back the respond message to the type you want */
		auto newState = c.call("takeDamage", exampleState).as<BaseState>();
		exampleState = newState;
		std::cout << "Player Health: " << exampleState.hp << std::endl;
	}

	if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Left))
	{
		/* Call echo rpc function that is defined on the server */
		/* Need to cast back the respond message to the type you want */
		auto newState = c.call("movePos", exampleState).as<BaseState>();
		exampleState = newState;
		std::cout << "Player Health: " << exampleState.pos.x << " " << exampleState.pos.y << " " << exampleState.pos.z << " " << std::endl;
	}
}