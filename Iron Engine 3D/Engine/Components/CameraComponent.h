#pragma once

#include "../Engine/Rendering/Camera/Camera.h"

struct CameraComponent
{
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	bool primary = true;

	glm::mat4 view;
	glm::mat4 projection;

	Camera engineCamera;
};