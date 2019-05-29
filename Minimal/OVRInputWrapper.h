#pragma once
#include <OVR_CAPI.h>

class OVRInputWrapper
{
	ovrInputState oldState;
	ovrInputState newState;
	const float TRIGGER_OFF_THRESHOLD{ 0.6f };
	const float TRIGGER_ON_THRESHOLD{ 0.6f };

public:
	OVRInputWrapper();

	static OVRInputWrapper& getInstance();

	void update(ovrInputState inputState);

	bool buttonPressed(unsigned button);

	bool buttonHeld(unsigned button);

	bool buttonReleased(unsigned button);

	bool indexTriggerPressed(int hand);

	bool indexTriggerHeld(int hand);

	bool indexTriggerReleased(int hand);

	bool gripTriggerPressed(int hand);

	bool gripTriggerHeld(int hand);

	bool gripTriggerReleased(int hand);

	float thumbStickHorizontal(int hand);

	float thumbStickVertical(int hand);
};

