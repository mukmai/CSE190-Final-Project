#include "OVRInputWrapper.h"

OVRInputWrapper::OVRInputWrapper()
{
}

OVRInputWrapper& OVRInputWrapper::getInstance()
{
	static OVRInputWrapper wrapper;
	return wrapper;
}

void OVRInputWrapper::update(ovrInputState inputState)
{
	oldState = newState;
	newState = inputState;
}

bool OVRInputWrapper::buttonPressed(unsigned button)
{
	return (!(oldState.Buttons & button) && (newState.Buttons & button));
}

bool OVRInputWrapper::buttonHeld(unsigned button)
{
	return newState.Buttons & button;
}

bool OVRInputWrapper::buttonReleased(unsigned button)
{
	return ((oldState.Buttons & button) && !(newState.Buttons & button));
}

bool OVRInputWrapper::indexTriggerPressed(int hand)
{
	return (oldState.IndexTrigger[hand] < TRIGGER_ON_THRESHOLD && newState.IndexTrigger[hand] >= TRIGGER_ON_THRESHOLD);
}

bool OVRInputWrapper::indexTriggerHeld(int hand)
{
	return oldState.IndexTrigger[hand] >= TRIGGER_ON_THRESHOLD;
}

bool OVRInputWrapper::indexTriggerReleased(int hand)
{
	return (oldState.IndexTrigger[hand] >= TRIGGER_OFF_THRESHOLD && newState.IndexTrigger[hand] < TRIGGER_OFF_THRESHOLD);
}

bool OVRInputWrapper::gripTriggerPressed(int hand)
{
	return (oldState.HandTrigger[hand] < TRIGGER_ON_THRESHOLD && newState.HandTrigger[hand] >= TRIGGER_ON_THRESHOLD);
}

bool OVRInputWrapper::gripTriggerHeld(int hand)
{
	return oldState.HandTrigger[hand] >= TRIGGER_ON_THRESHOLD;
}

bool OVRInputWrapper::gripTriggerReleased(int hand)
{
	return (oldState.HandTrigger[hand] >= TRIGGER_OFF_THRESHOLD && newState.HandTrigger[hand] < TRIGGER_OFF_THRESHOLD);
}

float OVRInputWrapper::thumbStickHorizontal(int hand)
{
	return newState.Thumbstick[hand].x;
}

float OVRInputWrapper::thumbStickVertical(int hand)
{
	return newState.Thumbstick[hand].y;
}

float OVRInputWrapper::gripTriggerRate(int hand)
{
	return newState.HandTrigger[hand];
}
