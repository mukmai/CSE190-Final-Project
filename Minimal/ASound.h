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

	  glm::vec3 sound_pos = glm::vec3(0.0f);
	  glm::vec3 sound_vel = glm::vec3(0.0f);

	  glm::mat4 storedListMat = glm::mat4(1.0f);

      // Constructors
      ASound();
      ASound(const char * filepath);

	  // General update function to use per-frame
	  void update();

	  // Update function to update the listener's (ie. Player's) attributes (ex. player position)
	  void update3DListener(glm::mat4 listMat);

	  // Update function to update the sound's attributes (ex. The sound's location)
	  void update3DChannel(glm::vec3 pos);
      
	  // Load the soundfile using just the filepath to the file
      void loadSound(const char * filepath);
      
	  // General function to simply play the loaded sound
      void playSound(float volume);

	  // TODO: Set up the sound for proper spatialization
      void playSound3D(float volume);

	  void playSound3DInstance(float volume);

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