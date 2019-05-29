#include "Proj1.h"
#include <ctime>
#include "OVRInputWrapper.h"

Proj1::Proj1()
{
}

void Proj1::initGl()
{
	RiftApp::initGl();
	glClearColor(0.7f, 0.7f, 0.7f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	sphereScene = std::shared_ptr<SpheresScene>(new SpheresScene());
	controllerScene = std::shared_ptr<ControllerSphereModule>(new ControllerSphereModule());
	srand(time(NULL));
}

void Proj1::shutdownGl()
{
	sphereScene.reset();
	controllerScene.reset();
}

void Proj1::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	controllerScene->render(projection, glm::inverse(headPose));
	sphereScene->render(projection, glm::inverse(headPose));
}

void Proj1::update()
{
	RiftApp::update();

	oldTime = newTime;
	newTime = ovr_GetTimeInSeconds();

	if (!gameStart)
	{
		if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right))
		{
			gameStart = true;
			sphereScene->curBall = rand() % 125;
			timer = BEGIN_TIME;
			score = 0;
		}
	} else
	{
		timer -= newTime - oldTime;
		if (timer <= 0)
		{
			sphereScene->curBall = -1;
			gameStart = false;
			printf("score: %d\n", score);
		}

		if (OVRInputWrapper::getInstance().indexTriggerPressed(ovrHand_Right))
		{
			if (checkHit())
			{
				sphereScene->curBall = rand() % 125;
				score++;
			}
		}
	}
	if (OVRInputWrapper::getInstance().gripTriggerPressed(ovrHand_Left) && sphereScene->withinBound(controllerPosition[0]))
	{
		grabbingSphere = true;
		origQuat = controllerRotation[0];
		origPos = controllerPosition[0];
		origT = glm::translate(glm::mat4(1.0f), (- origPos));
	}

	if (grabbingSphere)
	{
		glm::mat4 R = glm::toMat4(controllerRotation[0] * glm::inverse(origQuat));
		glm::mat4 T = glm::translate(glm::mat4(1.0f), (controllerPosition[0] - origPos));
		sphereScene->T = T * glm::inverse(origT) * R * origT;
	}
	if (OVRInputWrapper::getInstance().gripTriggerReleased(ovrHand_Left))
	{
		grabbingSphere = false;
		sphereScene->C = sphereScene->T * sphereScene->C;
		sphereScene->T = glm::mat4(1.0f);
	}
}

bool Proj1::checkHit()
{
	glm::vec3 ballPos = sphereScene->getBallPos(sphereScene->curBall);
	glm::vec3 temp = ballPos - controllerPosition[1];
	float dist = glm::length(temp);
	return (dist <= (sphereScene->scale + controllerScene->scale)/2);
}