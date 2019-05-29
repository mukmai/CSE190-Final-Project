#pragma once
#include <glm/mat4x2.hpp>
#include "Renderable.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>

const int TEXTURE_WIDTH = 1024;
const int TEXTURE_HEIGHT = 1024;

class CaveComponent : public Renderable
{
	std::vector<std::shared_ptr<Renderable>> renderList;
	GLuint _fbo;
	GLuint _texture;
	GLuint _rbo;
	GLenum DrawBuffers[1];
	GLuint VAO;
	GLuint VertexBuffer;
	GLuint UVBuffer;
	std::unique_ptr<Shader> shader;
	glm::vec3 pa; // bottom left corner pos
	glm::vec3 pb; // bottom right corner pos
	glm::vec3 pc; // top left corner pos
	glm::vec3 pd; // top right corner pos
	bool cornerUpdated;

	GLuint debugVAO;
	GLuint debugVBO;
	std::vector<glm::vec3> debugVertices;
	std::unique_ptr<Shader> debugShader;
	
public:
	glm::vec3 position;
	glm::mat4 r;
	glm::mat4 wr;
	glm::vec3 scale;
	static glm::vec3 trackingEyePos[2];
	static bool debugMode;
	static int currentEye;
	static bool fallOffLCD;
	static glm::vec3 realEyePos[2];
	int failedProjector;

	CaveComponent(glm::vec3 pos, glm::vec3 rotation, glm::vec3 scale, glm::vec3 wRotation);

	void render(const glm::mat4& projection, const glm::mat4& view) override;

	void addRenderList(std::shared_ptr<Renderable> entity);

	void initCaveGL();

	void updateCorners();

	glm::mat4 getProjection(glm::vec3 pe, float n, float f);

	void initDebugGL();

	void updateDebugVertices(glm::vec3 eyePos);

	void renderDebug(const glm::mat4& projection, const glm::mat4& view);
};

