﻿#include "RiftApp.h"
#include "OVRInputWrapper.h"

RiftApp::RiftApp()
{
	using namespace ovr;
	_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

	memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
	_sceneLayer.Header.Type = ovrLayerType_EyeFov;
	_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

	ovr::for_each_eye([&](ovrEyeType eye)
	{
		ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
		ovrMatrix4f ovrPerspectiveProjection =
			ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
		_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
		_viewScaleDesc.HmdToEyePose[eye] = erd.HmdToEyePose;

		ovrFovPort& fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
		auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
		_sceneLayer.Viewport[eye].Size = eyeSize;
		_sceneLayer.Viewport[eye].Pos = {(int)_renderTargetSize.x, 0};

		_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
		_renderTargetSize.x += eyeSize.w;
	});
	// Make the on screen window 1/4 the resolution of the render target
	_mirrorSize = _renderTargetSize;
	_mirrorSize /= 4;
}

GLFWwindow* RiftApp::createRenderingTarget(glm::uvec2& outSize, glm::ivec2& outPosition)
{
	return glfw::createWindow(_mirrorSize);
}

void RiftApp::initGl()
{
	GlfwApp::initGl();

	// Disable the v-sync for buffer swap
	glfwSwapInterval(0);

	ovrTextureSwapChainDesc desc = {};
	desc.Type = ovrTexture_2D;
	desc.ArraySize = 1;
	desc.Width = _renderTargetSize.x;
	desc.Height = _renderTargetSize.y;
	desc.MipLevels = 1;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleCount = 1;
	desc.StaticImage = ovrFalse;
	ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
	_sceneLayer.ColorTexture[0] = _eyeTexture;
	if (!OVR_SUCCESS(result))
	{
		FAIL("Failed to create swap textures");
	}

	int length = 0;
	result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
	if (!OVR_SUCCESS(result) || !length)
	{
		FAIL("Unable to count swap chain textures");
	}
	for (int i = 0; i < length; ++i)
	{
		GLuint chainTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
		glBindTexture(GL_TEXTURE_2D, chainTexId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Set up the framebuffer object
	glGenFramebuffers(1, &_fbo);
	glGenRenderbuffers(1, &_depthBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	ovrMirrorTextureDesc mirrorDesc;
	memset(&mirrorDesc, 0, sizeof(mirrorDesc));
	mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	mirrorDesc.Width = _mirrorSize.x;
	mirrorDesc.Height = _mirrorSize.y;
	if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture)))
	{
		FAIL("Could not create mirror texture");
	}
	glGenFramebuffers(1, &_mirrorFbo);
}

void RiftApp::onKey(int key, int scancode, int action, int mods)
{
	if (GLFW_PRESS == action)
		switch (key)
		{
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

	GlfwApp::onKey(key, scancode, action, mods);
}

void RiftApp::draw()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyePose, eyePoses, &_sceneLayer.SensorSampleTime);

	int curIndex;
	ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
	GLuint curTexId;
	ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);

	setEyeTracking();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ovr::for_each_eye([&](ovrEyeType eye)
	{
		renderingEye = eye;
		const auto& vp = _sceneLayer.Viewport[eye];
		glViewport(vp.Pos.x, vp.Pos.y, vp.Size.w, vp.Size.h);
		_sceneLayer.RenderPose[eye] = eyePoses[eye]; // set where the quad (screen) render
		renderScene(_eyeProjections[eye], ovr::toGlm(eyePoses[eye]));  // app render
	});
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	ovr_CommitTextureSwapChain(_session, _eyeTexture);
	ovrLayerHeader* headerList = &_sceneLayer.Header;
	ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

	GLuint mirrorTextureId;
	ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
	glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT,
	                  GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void RiftApp::update()
{
	// get the general state hmdState
	displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, 0);
	_hmdState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);
	for (int i = 0; i < 2; i++) // left hand = 0, right hand = 1
	{
		// Get the state of hand poses
		handPoseState = _hmdState.HandPoses[i];
		//Get the hand pose position
		controllerPosition[i] = ovr::toGlm(handPoseState.ThePose.Position);
		// Get hand rotation
		controllerRotation[i] = ovr::toGlm(ovrQuatf(handPoseState.ThePose.Orientation));
	}

	// get input
	ovr_GetInputState(_session, ovrControllerType_Touch, &inputState);
	OVRInputWrapper::getInstance().update(inputState);

}

void RiftApp::setEyeTracking() {
}