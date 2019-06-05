#include "RiftApp.h"

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <OVR_CAPI.h>

#include <vector>

#include <random>



struct Particle{
    glm::vec3 pos;
    glm::vec3 vel;

    glm::vec4 color;

    float size, angle, weight;
    float life;

    float camDist;

    bool operator<(Particle & other){
        return this->camDist > other.camDist;
    }
};


const glm::vec3 vertices[] = {
	glm::vec3(-0.5, -0.5f, 0.0f),
	glm::vec3(0.5, -0.5f, 0.0f),
	glm::vec3(-0.5, 0.5f, 0.0f),
	glm::vec3(0.5, 0.5f, 0.0f),
};


const unsigned int MAX_PARTICLES = 1000;


class AParticleSystem{
  public:
    AParticleSystem();

    ~AParticleSystem();


    void init();

    void update(glm::vec3 origin, glm::vec3 eyePos);

    void render(glm::mat4 projMat, glm::mat4 camMat);


  private:
    double lastTime, newTime, deltaTime;

    unsigned int numParticles;
    unsigned int lastParticle;

    GLuint verticesBO;
    GLuint positionBO;
    GLuint colorBO;

	
	
	Particle container[MAX_PARTICLES];
	glm::vec3 positions[MAX_PARTICLES];
	glm::vec4 colors[MAX_PARTICLES];

    void initGL();

    float randomFloat(float lb, float ub);

    glm::vec3 randomVec3(float radius);

    unsigned int findUnusedParticle();

    void sortParticles();


};
