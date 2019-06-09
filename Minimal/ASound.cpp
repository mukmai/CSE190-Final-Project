#include "ASound.h"

ASound::ASound(){
	fmod_result = FMOD::System_Create(&system);     
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: ASound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
    fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: ASound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

ASound::ASound(const char * filepath){
	fmod_result = FMOD::System_Create(&system);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: ASound(const char * filepath)");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
	fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: ASound(const char * filepath)");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

    loadSound(filepath);
}

void ASound::update() {
	system->update();
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: update()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::updateListener(glm::mat4 listenerMat) {
	// Set listener position
	const FMOD_VECTOR fPos = glmToFMOD(listenerMat[3]);

	// Set up listener orientation
	glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 forward = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	// Clear the last column of the listener matrix
	glm::mat4 rotMat = listenerMat;
	rotMat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Simply rotate the vectors by the above rotation matrix
	up      = rotMat * up;
	forward = rotMat * forward;

	// Convert the vectors to FMOD vectors
	const FMOD_VECTOR fUp      = glmToFMOD(up);
	const FMOD_VECTOR fForward = glmToFMOD(forward);

	// For now, assume velocity is 0 m/s
	const FMOD_VECTOR fVel = glmToFMOD(glm::vec3(0.0f));

	// Set 3D attributes for the player listener
	fmod_result = system->set3DListenerAttributes(0, &fPos, &fVel, &fForward, &fUp);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: updateListener()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::loadSound(const char * filepath){
	// Just create the sound using the filepath. 
    fmod_result = system->createSound(filepath, FMOD_3D, NULL, &sound);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: loadSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

}

void ASound::playSound(float volume){
	bool bPlaying;
	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: playSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		return;
	}

	// Just play the stored sound
    fmod_result = system->playSound(sound, NULL, false, &channel);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: stopSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Set the volume of the sound
	fmod_result = channel->setVolume(volume);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: stopSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::playSound3D(glm::vec3 position){
	bool bPlaying;
	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: playSound3D()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		return;
	}

	// Play the stored sound
	fmod_result = system->playSound(sound, NULL, false, &channel);	
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: playSound3D()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Modify channel information using the given position
	FMOD_VECTOR fPos = glmToFMOD(position);
	FMOD_VECTOR fVel = glmToFMOD(glm::vec3(100.0f, 100.0f, 100.0f));

	fmod_result = channel->set3DAttributes(&fPos, NULL);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: playSound3D()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::pauseSound() {
	fmod_result = channel->setPaused(true);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: pauseSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::unpauseSound() {
	fmod_result = channel->setPaused(true);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: unpauseSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::stopSound() {
	bool bPlaying;

	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: stopSound()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		fmod_result = channel->stop();
		if (fmod_result != FMOD_OK)
		{
			printf("FUNCTION: stopSound()");
			printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		}

		//FMOD::Channel* pChannel = nullptr;
		//fmod_result = system->getChannel(0, &pChannel);

		//if (fmod_result == FMOD_OK && pChannel)
		//{
		//	pChannel->stop();
		//	if (fmod_result != FMOD_OK)
		//	{
		//		printf("FUNCTION: stopSound()");
		//		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		//	}
		//}
		//else if(fmod_result != FMOD_OK)
		//{
		//	printf("FUNCTION: stopSound()");
		//	printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		//}
	}
}

FMOD_VECTOR ASound::glmToFMOD(glm::vec3 gvec){
	// Create a new vector, set xyz, then return the vector
    FMOD_VECTOR fvec;
    fvec.x = gvec.x;
    fvec.y = gvec.y;
    fvec.z = gvec.z;
    return fvec;
}

void ASound::setVolume(float volume) {
	pauseSound();

	fmod_result = system->getChannel(0, &channel);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: setVolume()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Set the volume of the sound
	fmod_result = channel->setVolume(volume);
	if (fmod_result != FMOD_OK)
	{
		printf("FUNCTION: setVolume()");
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	unpauseSound();
}