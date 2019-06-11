#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include "GlfwApp.h"
#include "RiftManagerApp.h"
#include <corecrt_memcpy_s.h>

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//
namespace glfw
{
	inline GLFWwindow* createWindow(const glm::uvec2& size, const glm::ivec2& position = glm::ivec2(INT_MIN)) {
		GLFWwindow* window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}


//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//
namespace ovr
{
	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline glm::mat4 toGlm(const ovrMatrix4f& om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline glm::mat4 toGlm(const ovrFovPort& fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline glm::vec3 toGlm(const ovrVector3f& ov) {
		return glm::make_vec3(&ov.x);
	}

	inline glm::vec2 toGlm(const ovrVector2f& ov) {
		return glm::make_vec2(&ov.x);
	}

	inline glm::uvec2 toGlm(const ovrSizei& ov) {
		return glm::uvec2(ov.w, ov.h);
	}

	inline glm::quat toGlm(const ovrQuatf& oq) {
		return glm::make_quat(&oq.x);
	}

	inline glm::mat4 toGlm(const ovrPosef& op) {
		glm::mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		glm::mat4 translation = glm::translate(glm::mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const glm::mat4& m) {
		ovrMatrix4f result;
		glm::mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const glm::vec3& v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const glm::vec2& v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const glm::uvec2& v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const glm::quat& q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftApp : public GlfwApp, public RiftManagerApp {
protected:
	ovrViewScaleDesc _viewScaleDesc;
	ovrTrackingState _hmdState;
	glm::uvec2 _mirrorSize;

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };
	ovrTextureSwapChain _eyeTexture;

	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;

	ovrEyeRenderDesc _eyeRenderDescs[2];

	glm::mat4 _eyeProjections[2];

	ovrLayerEyeFov _sceneLayer;
	

	glm::uvec2 _renderTargetSize;


public:

	RiftApp();

	double displayMidpointSeconds;
	ovrPoseStatef handPoseState;
	glm::vec3 controllerPosition[2];
	glm::quat controllerRotation[2];
	ovrInputState inputState;
	ovrEyeType renderingEye;
	ovrPosef eyePoses[2];
	
	

protected:
	GLFWwindow* createRenderingTarget(glm::uvec2& outSize, glm::ivec2& outPosition) override;

	void initGl() override;

	void onKey(int key, int scancode, int action, int mods) override;

	void draw() final override;

	virtual void renderScene(const glm::mat4& projection, const glm::mat4& headPose) = 0;

	void update() override;

	virtual void setEyeTracking();
};