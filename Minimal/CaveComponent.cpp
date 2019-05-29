#include "CaveComponent.h"
#include <iostream>

bool CaveComponent::debugMode;
int CaveComponent::currentEye;
glm::vec3 CaveComponent::trackingEyePos[2] = {};
bool CaveComponent::fallOffLCD;
glm::vec3 CaveComponent::realEyePos[2] = {};

const GLfloat vertices[] = {
	0.5f, -0.5f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f, // bottom left
	0.5f,  0.5f, 0.0f, // top right
	0.5f,  0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f // top left
};

const GLfloat uvs[] = {
	1.0f, 0.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	0.0f, 1.0f
};

CaveComponent::CaveComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, glm::vec3 wRotation)
{
	failedProjector = 2;
	debugMode = false;
	currentEye = 0;
	cornerUpdated = false;
	fallOffLCD = false;

	initCaveGL();
	this->position = position;

	glm::mat4 rx = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
	glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
	glm::mat4 rz = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
	this->r = rz * ry * rx;

	glm::mat4 wrx = glm::rotate(glm::mat4(1.0f), glm::radians(wRotation.x), glm::vec3(1, 0, 0));
	glm::mat4 wry = glm::rotate(glm::mat4(1.0f), glm::radians(wRotation.y), glm::vec3(0, 1, 0));
	glm::mat4 wrz = glm::rotate(glm::mat4(1.0f), glm::radians(wRotation.z), glm::vec3(0, 0, 1));
	this->wr = wrz * wry * wrx;

	this->scale = scale;

	initDebugGL();
}

void CaveComponent::render(const glm::mat4& projection, const glm::mat4& view) {

	// save previous framebuffer and viewport
	GLint old_fbo;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &old_fbo);
	GLint old_viewport[4];
	glGetIntegerv(GL_VIEWPORT, old_viewport);


	// bind our framebuffer and set viewport to size of texture
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get new projection matrix
	glm::mat4 newP = getProjection(trackingEyePos[currentEye], 0.01f, 1000.0f);

	// render scene
	for (auto &renderObj : renderList) {
		renderObj->render(newP, glm::mat4(1.0f));
	}

	// bind the previous framebuffer and viewport after drawing the texture
	glBindFramebuffer(GL_FRAMEBUFFER, old_fbo);
	glViewport(old_viewport[0], old_viewport[1], old_viewport[2], old_viewport[3]);



	// render quads with the texture
	shader->use();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	// compute model matrix based on state: t * r * s
	const auto t = glm::translate(glm::mat4(1.0f), position);
	const auto s = glm::scale(glm::mat4(1.0f), scale);
	auto model = wr * t * r * s;
	shader->setMat4("model", model);
	shader->setBool("fallOffLCD", fallOffLCD);
	shader->setVec3("eyePos", realEyePos[currentEye]);
	shader->setBool("failed", currentEye == failedProjector);
	
	// bind texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	// Set our "renderedTexture" sampler to use Texture Unit 0
	glUniform1i(glGetUniformLocation(shader->ID, "renderedTexture"), 0);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	if (debugMode) {
		renderDebug(projection, view);
	}
}

void CaveComponent::addRenderList(std::shared_ptr<Renderable> entity)
{
	renderList.push_back(entity);
}

void CaveComponent::initCaveGL() {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	// The texture we're going to render to
	glGenTextures(1, &_texture);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _texture);
	// Give an empty image to OpenGL ( the last NULL )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	// Set the list of draw buffers.
	DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	glGenRenderbuffers(1, &_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach the renderbuffer object (framebuffer target, attachment point, render buffer target, renderbuffer ID)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

	// make sure frame buffer setup completed
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "cave component initialize failed" << std::endl;


	// set up for quad rendering
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VertexBuffer);
	glGenBuffers(1, &UVBuffer);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, UVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader = std::make_unique<Shader>("./Shaders/cave.vert", "./Shaders/cave.frag");
}

void CaveComponent::updateCorners() {
	const auto t = glm::translate(glm::mat4(1.0f), position);
	const auto s = glm::scale(glm::mat4(1.0f), scale);
	auto model = wr * t * r * s;
	pa = model * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
	pb = model * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
	pc = model * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
	pd = model * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
}

glm::mat4 CaveComponent::getProjection(glm::vec3 pe, float n, float f) {
	if (!cornerUpdated) {
		cornerUpdated = true;
		updateCorners();
	}
	
	glm::vec3 va, vb, vc;
	glm::vec3 vr, vu, vn;
	float l, r, b, t, d;

	// Compute orthonormal basis for the screen
	vr = glm::normalize(pb - pa);
	vu = glm::normalize(pc - pa);
	vn = glm::normalize(glm::cross(vr, vu));

	// Compute the screen corner vectors
	va = pa - pe;
	vb = pb - pe;
	vc = pc - pe;

	// Find distance from eye to screen plane
	d = -glm::dot(vn, va);

	// Find the extent of the perpendicular projection
	l = glm::dot(vr, va) * n / d;
	r = glm::dot(vr, vb) * n / d;
	b = glm::dot(vu, va) * n / d;
	t = glm::dot(vu, vc) * n / d;

	// get frustum that is user-centered perspective (frustum lies in XY plane)
	glm::mat4 P = glm::frustum(l, r, b, t, n, f);

	// Obtain M (projection plane orientation) <-- same as viewport rotation
	glm::mat4 M(1.0f);
	M[0] = glm::vec4(vr, 0.0f);
	M[1] = glm::vec4(vu, 0.0f);
	M[2] = glm::vec4(vn, 0.0f);

	// Obtain T (view point offset)
	glm::mat4 T = glm::translate(glm::mat4(1.0f), -pe);

	return P * glm::transpose(M) * T;
}

void CaveComponent::updateDebugVertices(glm::vec3 eyePos)
{
	if (!cornerUpdated) {
		cornerUpdated = true;
		updateCorners();
	}
	debugVertices[0] = pa;
	debugVertices[1] = eyePos;
	debugVertices[2] = pb;
	debugVertices[3] = eyePos;
	debugVertices[4] = pc;
	debugVertices[5] = eyePos;
	debugVertices[6] = pd;
	debugVertices[7] = eyePos;
	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, debugVertices.size() * sizeof(glm::vec3), &debugVertices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CaveComponent::initDebugGL() {
	for (int i = 0; i < 8; ++i) {
		debugVertices.push_back(glm::vec3(0));
	}
	updateDebugVertices(glm::vec3(0, 0, 0));

	glGenVertexArrays(1, &debugVAO);
	glGenBuffers(1, &debugVBO);

	glBindVertexArray(debugVAO);

	glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
	glBufferData(GL_ARRAY_BUFFER, debugVertices.size() * sizeof(glm::vec3), &debugVertices[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	debugShader = std::make_unique<Shader>("./Shaders/line.vert", "./Shaders/line.frag");
}

void CaveComponent::renderDebug(const glm::mat4& projection, const glm::mat4& view) {
	for (int eye = 0; eye < 2; eye++) {
		updateDebugVertices(trackingEyePos[eye]);
		debugShader->use();
		debugShader->setMat4("projection", projection);
		debugShader->setMat4("view", view);
		debugShader->setMat4("model", glm::mat4(1.0f));
		debugShader->setInt("eye", eye);
		glBindVertexArray(debugVAO);
		glLineWidth(4.0f);
		glDrawArrays(GL_LINES, 0, (GLsizei)debugVertices.size());
		glBindVertexArray(0);
	}
}