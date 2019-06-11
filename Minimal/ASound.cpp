#include "ASound.h"

ASound::ASound(){
	fmod_result = FMOD::System_Create(&system);     
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: ASound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
    fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: ASound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

ASound::ASound(const char * filepath){
	fmod_result = FMOD::System_Create(&system);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: ASound(const char * filepath)");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
	fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: ASound(const char * filepath)");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

    loadSound(filepath);
}

void ASound::update() {
	system->update();
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: update()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::update3DListener(glm::mat4 listMat){
	glm::vec3 list_pos         = listMat[3];
	glm::vec3 list_vel         = list_pos - glm::vec3(storedListMat[3]);
	glm::vec3 list_cam_forward = glm::normalize(listMat * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	glm::vec3 list_cam_up      = glm::normalize(listMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));

	// Set listener attributes
	const FMOD_VECTOR fPos     = glmToFMOD(list_pos);
	const FMOD_VECTOR fVel     = glmToFMOD(list_vel);
	const FMOD_VECTOR fForward = glmToFMOD(list_cam_forward);
	const FMOD_VECTOR fUp      = glmToFMOD(list_cam_up);

	// Set 3D attributes for the player listener
	fmod_result = system->set3DListenerAttributes(0, &fPos, &fVel, &fForward, &fUp);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: update3DListener()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Update the system
	fmod_result = system->update();
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: update3DListener()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	storedListMat = listMat;
}

void ASound::update3DChannel(glm::vec3 ch_pos) {
	glm::vec3 ch_vel = ch_pos - sound_pos;
	const FMOD_VECTOR fPos = glmToFMOD(ch_pos);
	const FMOD_VECTOR fVel = glmToFMOD(ch_vel);

	fmod_result = channel->set3DAttributes(&fPos, &fVel);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: update3DChannel()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	system->update();
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: update3DChannel()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	sound_pos = ch_pos;
}

void ASound::loadSound(const char * filepath){
	// Just create the sound using the filepath. 
    fmod_result = system->createSound(filepath, FMOD_3D, NULL, &sound);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: loadSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

}

void ASound::playSound(float volume){
	bool bPlaying;
	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: playSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		return;
	}

	// Just play the stored sound
    fmod_result = system->playSound(sound, NULL, false, &channel);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: stopSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Set the volume of the sound
	fmod_result = channel->setVolume(volume);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: stopSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::playSound3D(float volume){
	bool bPlaying;
	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: playSound3D()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		return;
	}

	// Play the stored sound
	fmod_result = system->playSound(sound, NULL, false, &channel);	
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: playSound3D()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Set the volume of the sound
	fmod_result = channel->setVolume(volume);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: stopSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::pauseSound() {
	fmod_result = channel->setPaused(true);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: pauseSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::unpauseSound() {
	fmod_result = channel->setPaused(true);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: unpauseSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}
}

void ASound::stopSound() {
	bool bPlaying;

	fmod_result = channel->isPlaying(&bPlaying);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: stopSound()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	if (bPlaying) {
		fmod_result = channel->stop();
		if (fmod_result != FMOD_OK)
		{
			//printf("FUNCTION: stopSound()");
			//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
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
		//printf("FUNCTION: setVolume()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	// Set the volume of the sound
	fmod_result = channel->setVolume(volume);
	if (fmod_result != FMOD_OK)
	{
		//printf("FUNCTION: setVolume()");
		//printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
	}

	unpauseSound();
}