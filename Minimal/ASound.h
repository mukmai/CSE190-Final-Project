#ifndef ASOUND_H
#define ASOUND_H

#include <glm/glm.hpp>

#include "OVRInputWrapper.h"
#include "RiftApp.h"

#include "fmod/fmod.hpp"
#include "fmod/fmod_errors.h"

// Attachable Sound Object
class ASound{
    public:
      // Ptrs to different components of FMOD sound API
	  FMOD::System  * system  = NULL;
	  FMOD::Sound   * sound   = NULL;
	  FMOD::Channel * channel = NULL;

      // Constructors
      ASound();
      ASound(const char * filepath);

	  // General update function to use per-frame
	  void update();

	  // Specific update function to update the listener's position (ie. player position)
	  void updateListener(glm::mat4 listenerMat);
      
	  // Load the soundfile using just the filepath to the file
      void loadSound(const char * filepath);
      
	  // General function to simply play the loaded sound
      void playSound(float volume);

	  // TODO: Set up the sound for proper spatialization
      void playSound3D(glm::vec3 position);

	  void pauseSound();

	  void unpauseSound();

	  void stopSound();

	  void setVolume(float volume);
    
    protected:
    
	  // Used to store the results (Success/Failure) of FMOD functions
      FMOD_RESULT fmod_result;
      
	  // Simple function to create FMOD vectors from GLM vectors
      FMOD_VECTOR glmToFMOD(glm::vec3 gvec);
};

#endif