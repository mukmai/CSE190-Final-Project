#include "AParticleSystem.h"


AParticleSystem::AParticleSystem(){
    // Seed the random number generator
    srand((unsigned int)(ovr_GetTimeInSeconds));

    init();
}

AParticleSystem::~AParticleSystem(){
    // TODO: delete any pointers and structs

    // TODO: delete the GL buffers
}

void AParticleSystem::init(){
    initGL();
}

void AParticleSystem::initGL(){
    glGenBuffers(1, &verticesBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &positionBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &colorBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void AParticleSystem::update(glm::vec3 origin, glm::vec3 eyePos){
    // TODO: Update timers
    lastTime = newTime;
    newTime  = ovr_GetTimeInSeconds();
    double deltaTime = newTime - lastTime;

    // TODO: Calculate the number of particles to update
    //        - Refresh expired particles
    unsigned int newParticles = (unsigned int)(deltaTime * 10000.0);
    if(newParticles > (unsigned int)(0.0111 * 10000.0)){
       newParticles > (unsigned int)(0.0111 * 10000.0);
    }

    // TODO: Create "numParticles" amount of particles
    for(unsigned int u = 0; u < numParticles; u++){
        unsigned int idx = findUnusedParticle();

        // Basic components of the particle
        container[idx].life = 7.0f;
        container[idx].pos  = origin;
		container[idx].size = 0.5f * randomFloat(0.1f, 10.0f);
		container[idx].color = glm::vec4(randomVec3(1.0f), 0.3f);

        // Figure out velocity vectors any specific particle
        float spread = 5.0f;
        glm::vec3 baseDir = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 randDir = randomVec3(container[idx].size);
        container[idx].vel = baseDir + glm::vec3(randDir*spread);
    }

    // TODO: Simulate all the particles
    int particleCount = 0;
    for(unsigned int u = 0; u < MAX_PARTICLES; u++){
		Particle & p = container[u];

		if (p.life > 0.0f) {
			p.life -= deltaTime;

			if (p.life > 0.0f) {
				p.vel += glm::vec3(0.0f, -9.81f, 0.0f) * (float)deltaTime * 0.5f;
				p.pos += p.vel * (float)deltaTime;
				p.camDist = glm::length(p.pos - eyePos);

				// Update GPU buffers
				positions[particleCount] = glm::vec4(p.pos, p.size);
				colors[particleCount] = p.color;
			}
			else {
				p.camDist = 1.0f;
			}

		}

		particleCount++;
    }

    // TODO: Sort the particles by their distance to the camera
    sortParticles();
    
    // Update the OpenGL buffers
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec4), NULL, positions);

    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, MAX_PARTICLES * sizeof(glm::vec4), NULL, colors);
}

void AParticleSystem::render(glm::mat4 projMat, glm::mat4 camMat){
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);
}

float AParticleSystem::randomFloat(float lb, float ub){
    float retval = lb + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(ub-lb)));
    return retval;
}

glm::vec3 AParticleSystem::randomVec3(float radius){
    glm::vec3 retvec;

    float phi = randomFloat(0, 2 * glm::pi<float>());
    float costheta = randomFloat(-1, 1);
    float u = randomFloat(0, 1);

    float theta = glm::acos(costheta);
    float r = radius * std::cbrt(u);

    retvec.x = r * glm::sin(theta) * glm::cos(phi);
    retvec.y = r * glm::sin(theta) * glm::sin(phi);
    retvec.z = r * cos(theta);

    return retvec;
}

unsigned int AParticleSystem::findUnusedParticle(){
    for(unsigned int u = lastParticle; u < MAX_PARTICLES; u++){
        if(container[u].life <= 0.0f){
            lastParticle = u;
            return u;
        }
    }

    for(unsigned int u = 0; u < lastParticle; u++){
        if(container[u].life <= 0.0f){
            lastParticle = u;
            return u;
        }
    }

    return 0;
}

void AParticleSystem::sortParticles(){
  std::sort(&container[0], &container[MAX_PARTICLES]);
}
