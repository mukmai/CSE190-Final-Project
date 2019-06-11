#include "AParticleSystem.h"

#include <glm/gtx/string_cast.hpp> 
#include <glm/gtx/norm.hpp>

AParticleSystem::AParticleSystem(){
    // Seed the random number generator
    //srand((unsigned int)(ovr_GetTimeInSeconds));

	shader = new Shader("./Resources/Shaders/particle_shader.vert", "./Resources/Shaders/particle_shader.frag");
	shader->use();

	sound = new ASound();

    init();

	newTime = std::chrono::system_clock::now();
}

AParticleSystem::~AParticleSystem(){
    // TODO: delete any pointers and structs
	//delete(container);
	//delete(positions);
	//delete(colors);

	//delete(sound);
	//delete(shader);
	
    // TODO: delete the GL buffers
	//glDeleteBuffers(1, &verticesBO);
	//glDeleteBuffers(1, &positionBO);
	//glDeleteBuffers(1, &colorBO);
	//glDeleteVertexArrays(1, &VAO);
}

void AParticleSystem::init(){
    initGL();
}

void AParticleSystem::initGL(){
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

    glGenBuffers(1, &verticesBO);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &positionBO);
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);

    glGenBuffers(1, &colorBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, verticesBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);
}

void AParticleSystem::update(glm::vec3 eyePos){
    // STEP: Update timers
	lastTime = newTime;
    newTime  = std::chrono::system_clock::now();
    unsigned int deltaTimeMS = std::chrono::duration_cast<std::chrono::milliseconds>(newTime - lastTime).count();
	double deltaTime = (double)deltaTimeMS / 1000.0;
	
	//std::cout << "***PARTICLE SHADER: delta time in frame: " << deltaTime << std::endl;
	//std::cout << "***PARTICLE SHADER: Origin of particle system: " << glm::to_string(origin) << std::endl;

    // STEP: Calculate the number of particles to update
    //        - Refresh expired particles
    unsigned int newParticles = (unsigned int)(deltaTime * 1000.0);
	if (newParticles > (unsigned int)(40)) {
		//std::cout << "***PARTICLE_SHADER: Limiting number of particles generated this frame." << std::endl;
        newParticles = (unsigned int)(40);
    }

	if (!bSpawnParticles) {
		newParticles = 0;
	}

	//std::cout << "***PARTICLE_SHADER: Generating " << newParticles << " particles." << std::endl;

    // STEP: Create "newParticles" amount of particles
	glm::mat4 rotMat = matModel;
	rotMat[3] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    for(unsigned int u = 0; u < newParticles; u++){
        unsigned int idx = findUnusedParticle();

        // Basic components of the particle
        container[idx].life = MAX_LIFESPAN;
        container[idx].pos  = matModel[3];
		container[idx].initSize = randomFloat(0.1f, 0.35f);
		/*glm::vec3 randColor = glm::vec3(
			randomFloat(0.4f, 0.6f),
			randomFloat(0.6f, 0.8f),
			randomFloat(0.8f, 1.0f)
		);*/
		glm::vec3 randColor = glm::vec3(
			randomFloat(0.4f, 1.0f),
			randomFloat(0.4f, 1.0f),
			randomFloat(0.4f, 1.0f)
		);
		container[idx].color = glm::vec4(randColor, 1.0f);

        // Figure out velocity vectors for any specific particle
        float spread = 0.55f;
        glm::vec3 baseDir = rotMat * glm::vec4(vecSpawnDir, 1.0f);
		//glm::vec3 baseDir = rotMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec3 randDir = glm::vec3(
			randomFloat(-4.0f, 4.0f),
			randomFloat(-4.0f, 4.0f),
			randomFloat(-1.0f, 1.0f)
		);
        container[idx].vel = baseDir + glm::vec3(randDir*spread);
    }

    // STEP: Simulate all the particles
    int particleCount = 0;
    for(unsigned int u = 0; u < MAX_PARTICLES; u++){
		Particle & p = container[u];

		if (p.life > 0.0f) {
			p.life -= deltaTime;

			if (p.life > 0.0f) {
				p.vel  = p.vel + (glm::vec3(0.0f, -9.81f, 0.0f) * ((float)deltaTime * 0.4f));
				p.pos  = p.pos + p.vel * (float)deltaTime;
				p.size    = p.initSize * (p.life / MAX_LIFESPAN);  // Shrink the particle as time goes on
				p.color.w = (p.life / MAX_LIFESPAN);               // Increase the transparency of the particle as time goes on
				p.camDist = glm::length2(p.pos - eyePos);

				// Update GPU buffers
				positions[particleCount] = glm::vec4(p.pos, p.size);
				colors[particleCount]    = p.color;

				
			}
			else {
				p.camDist = -1.0f;
			}

			particleCount++;
		}
    }

    // STEP: Sort the particles by their distance to the camera
    sortParticles();

	glBindVertexArray(VAO);
    
    // STEP: Update the OpenGL buffers
    glBindBuffer(GL_ARRAY_BUFFER, positionBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * particleCount, &positions[0]);

    glBindBuffer(GL_ARRAY_BUFFER, colorBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * MAX_PARTICLES, NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4) * particleCount, &colors[0]);

	glBindVertexArray(0);

	numParticles = particleCount;
}

void AParticleSystem::update3DAudio(glm::mat4 listMat) {
	// Update the listener attributes of the sound (ie. What the sound knows about the lister/player)
	sound->update3DListener(listMat);

	// Update the 3D attributes of the audio channel of the sound itself
	glm::vec3 currPos = matModel[3];
	sound->update3DChannel(currPos);
	lastPos = currPos;
}

void AParticleSystem::render(glm::mat4 projMat, glm::mat4 camMat){
	
	shader->use();

	shader->setMat4("projMat", projMat);
	shader->setMat4("camMat", camMat);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(VAO);

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numParticles);

	glBindVertexArray(0);

	glDisable(GL_BLEND);
}

float AParticleSystem::randomFloat(float lb, float ub){
	if ((ub - lb) == 0.0f) {
		return 0.0f;
	}
	else {
		float retval = lb + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (ub - lb)));
		return retval;
	}
}

glm::vec3 AParticleSystem::randomVec3(float lb, float ub) {
	glm::vec3 ret_vec = glm::vec3(
		randomFloat(lb, ub),
		randomFloat(lb, ub),
		randomFloat(lb, ub)
	);
	return ret_vec;
}

glm::vec3 AParticleSystem::randomSphereVec3(float radius){
    glm::vec3 retvec;

    float phi = randomFloat(0.0f, 2.0f * glm::pi<float>());
    float costheta = randomFloat(-1.0f, 1.0f);
    float u = randomFloat(0.0f, 1.0f);

    float theta = glm::acos(costheta);
    float r = radius * std::cbrt(u);

    retvec.x = r * glm::sin(theta) * glm::cos(phi);
    retvec.y = r * glm::sin(theta) * glm::sin(phi);
    retvec.z = r * cos(theta);

    return retvec;
}

glm::vec3 AParticleSystem::randomBoxVec3(glm::vec3 ctr, glm::vec3 ofs) {
	glm::vec3 ret_vec = glm::vec3(
		randomFloat(ctr.x - ofs.x, ctr.x + ofs.x),
		randomFloat(ctr.y - ofs.y, ctr.y + ofs.y),
		randomFloat(ctr.z - ofs.z, ctr.z + ofs.z)
		);

	return ret_vec;
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

void AParticleSystem::setSound(const char * filepath) {
	sound->loadSound(filepath);
}


void AParticleSystem::playPS() {
	bSpawnParticles = true;

	if (bPlaySound) {
		sound->playSound3D(1.0f);
	}
}


void AParticleSystem::stopPS() {
	bSpawnParticles = false;
	sound->stopSound();
}