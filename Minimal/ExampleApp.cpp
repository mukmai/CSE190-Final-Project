#include "ExampleApp.h"

ExampleApp::ExampleApp()
{
}

void ExampleApp::initGl()
{
	RiftApp::initGl();
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f); // background color
	glEnable(GL_DEPTH_TEST);
	ovr_RecenterTrackingOrigin(_session);
	cubeScene = std::shared_ptr<ColorCubeScene>(new ColorCubeScene());
}

void ExampleApp::shutdownGl()
{
	cubeScene.reset();
}

void ExampleApp::renderScene(const glm::mat4& projection, const glm::mat4& headPose)
{
	cubeScene->render(projection, glm::inverse(headPose));
}
