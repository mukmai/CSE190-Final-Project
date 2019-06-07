#pragma once
#include "rpc/client.h"
#include "RiftApp.h"
#include "SpheresScene.h"
#include "ControllerSphereModule.h"

//#include <vector>

#include "BaseState.h"
#include "Common.h"
#include "PlayerController.h"

#include "ASound.h"
#include "AParticleSystem.h"

#include <iostream>

// An example application that renders a simple cube
class Client : public RiftApp {
	double oldTime = 0;
	double newTime = 0;
	rpc::client c;
	std::shared_ptr<SpheresScene> sphereScene;
	PlayerController playerController;

	std::vector<ASound *> bgmList;
	ASound * soundBGM;
	//ASound * soundBGM = new ASound("Resources/Audio/BGM/bgm1.mp3");

	ASound * soundFire = new ASound("Resources/Audio/SoundEffects/fire1.wav");

	AParticleSystem * leftPS;
	AParticleSystem * rightPS;

public:
	Client();

protected:
	void initGl() override;

	void initAudio();

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;
};