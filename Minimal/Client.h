#pragma once
#include "rpc/client.h"
#include "RiftApp.h"
#include "SpheresScene.h"
#include "ControllerSphereModule.h"

#include "BaseState.h"
#include "Common.h"
#include "PlayerController.h"

#include "ASound.h"

// An example application that renders a simple cube
class Client : public RiftApp {
	double oldTime = 0;
	double newTime = 0;
	rpc::client c;
	std::shared_ptr<SpheresScene> sphereScene;
	PlayerController playerController;

	ASound * soundBGM = new ASound("Resources/Audio/BGM/bgm5.wav");
	ASound * soundFire = new ASound("Resources/Audio/SoundEffects/fire1.wav");

public:
	Client();

protected:
	void initGl() override;

	void initAudio();

	void shutdownGl() override;

	void renderScene(const glm::mat4& projection, const glm::mat4& headPose) override;

	void update() override;
};