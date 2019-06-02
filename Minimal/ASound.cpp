#include "ASound.h"

ASound::ASound(){
	fmod_result = FMOD::System_Create(&system);     
    fmod_result = system->init(512, FMOD_INIT_3D_RIGHTHANDED, 0);
}

ASound::ASound(const char * filepath){
    ASound();
    loadSound(filepath);
}

void ASound::loadSound(const char * filepath){
    fmod_result = system->createSound(filepath, FMOD_3D, NULL, &sound);
}

void ASound::playSound(){
    fmod_result = system->playSound(sound, NULL, FMOD_3D, NULL);
}

void ASound::playSound3D(){
    fmod_result = system->playSound(sound, NULL, FMOD_3D, NULL);
}

FMOD_VECTOR ASound::glmToFMOD(glm::vec3 gvec){
    FMOD_VECTOR fvec;
    fvec.x = gvec.x;
    fvec.y = gvec.y;
    fvec.z = gvec.z;
    
    return fvec;
}