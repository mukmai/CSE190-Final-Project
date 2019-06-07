#include "RiftApp.h"

#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <OVR_CAPI.h>

#include <vector>

#include <chrono>
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
	glm::vec3(-0.05f, -0.05f, 0.0f),
	glm::vec3(0.05f, -0.05f, 0.0f),
	glm::vec3(-0.05f, 0.05f, 0.0f),
	glm::vec3(0.05f, 0.05f, 0.0f),
};


const unsigned int MAX_PARTICLES = 100;
const float MAX_LIFESPAN = 5.0f;


class AParticleSystem{
  public:
    AParticleSystem();

    ~AParticleSystem();


    void init();

    void update(glm::vec3 origin_pos, glm::mat4 origin_rot, glm::vec3 eyePos);

    void render(glm::mat4 projMat, glm::mat4 camMat);


  private:
	std::chrono::system_clock::time_point newTime, lastTime;

    unsigned int numParticles;
    unsigned int lastParticle;

	GLuint VAO;
    GLuint verticesBO;
    GLuint positionBO;
    GLuint colorBO;

	Shader * shader;
	
	Particle container[MAX_PARTICLES];
	glm::vec3 positions[MAX_PARTICLES];
	glm::vec4 colors[MAX_PARTICLES];

	glm::vec3 OFFSET_VEC = 0.3f * glm::vec3(1.0f, 1.0f, 1.0f);

    void initGL();

    float randomFloat(float lb, float ub);

	glm::vec3 randomVec3(float lb, float ub);
    glm::vec3 randomSphereVec3(float radius);
	glm::vec3 randomBoxVec3(glm::vec3 ctr, glm::vec3 ofs);

    unsigned int findUnusedParticle();

    void sortParticles();


};
