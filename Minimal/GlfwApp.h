#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define FAIL(X) throw std::runtime_error(X)

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	glm::uvec2 windowSize;
	glm::ivec2 windowPosition;
	GLFWwindow* window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp();

	virtual ~GlfwApp();

	virtual int run();

protected:
	virtual GLFWwindow* createRenderingTarget(glm::uvec2& size, glm::ivec2& pos) = 0;

	virtual void draw() = 0;

	void preCreate();

	void postCreate();

	virtual void initGl();

	virtual void shutdownGl();

	virtual void finishFrame();

	virtual void destroyWindow();

	virtual void onKey(int key, int scancode, int action, int mods);

	virtual void update();

	virtual void onMouseButton(int button, int action, int mods);

protected:
	virtual void viewport(const glm::ivec2& pos, const glm::uvec2& size);

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static void ErrorCallback(int error, const char* description);
};