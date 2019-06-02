#ifndef ASOUND_H
#define ASOUND_H

#include <glm/glm.hpp>

#include "fmod/fmod.hpp"

// Attachable Sound Object
class ASound{
    public:
      
      ASound();
      ASound(const char * filepath);
      
      void loadSound(const char * filepath);
      
      void playSound();
      void playSound3D();
      
    
    protected:
    
      FMOD_RESULT fmod_result;
      FMOD::System * system = NULL;
      FMOD::Sound *  sound  = NULL;
      
      FMOD_VECTOR glmToFMOD(glm::vec3 gvec);
      
};

#endif