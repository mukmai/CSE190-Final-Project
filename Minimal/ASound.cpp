#include "ASound.h"

ASound::ASound(){
	fmod_result = FMOD::System_Create(&system);     

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
    fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}
}

ASound::ASound(const char * filepath){
	fmod_result = FMOD::System_Create(&system);

	// FMOD's normal initialization is done in left-hand coordinates 
	// (+z goes into the screen) but OpenGL is in right-handed coordinates
	// (+z goes out of the screen).
	fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}

    loadSound(filepath);
}

void ASound::update() {
	system->update();
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
}

void ASound::loadSound(const char * filepath){
	// Just create the sound using the filepath. 
    fmod_result = system->createSound(filepath, FMOD_3D, NULL, &sound);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}

}

void ASound::playSound(){
	// Just play the stored sound
    fmod_result = system->playSound(sound, NULL, false, NULL);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}
}

void ASound::playSound3D(glm::vec3 position){
	// Play the stored sound
	fmod_result = system->playSound(sound, NULL, false, &channel);	
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}

	// Modify channel information using the given position
	FMOD_VECTOR fPos = glmToFMOD(position);
	FMOD_VECTOR fVel = glmToFMOD(glm::vec3(100.0f, 100.0f, 100.0f));

	fmod_result = channel->set3DAttributes(&fPos, NULL);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}
}

void ASound::pauseSound() {
	channel->setPaused(true);
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
	}
}

void ASound::stopSound() {
	channel->stop();
	if (fmod_result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", fmod_result, FMOD_ErrorString(fmod_result));
		exit(-1);
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