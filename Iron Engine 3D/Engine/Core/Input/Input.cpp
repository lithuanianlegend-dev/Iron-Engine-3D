#include "Input.h"

#include "../Engine/Rendering/Camera/Camera.h"
#include "../Engine/Components/CameraComponent.h"
#include "../Engine/Core/Time/Time.h"

namespace IronEngine
{
	InputHandler::InputHandler() {  }

	void InputHandler::Poll(GLFWwindow* window, entt::registry& registry, entt::entity camera)
	{
		bool isSprinting = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			Core.DestroyEngine();

		float deltaTime = Time.GetDeltaTime();

		auto& cameraComp = registry.get<CameraComponent>(camera);


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			cameraComp.engineCamera.canRotate = true;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cameraComp.engineCamera.canRotate = false;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(FORWARD, deltaTime, isSprinting);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(BACKWARD, deltaTime, isSprinting);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(LEFT, deltaTime, isSprinting);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(RIGHT, deltaTime, isSprinting);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(UP, deltaTime, isSprinting);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cameraComp.engineCamera.ProcessKeyboard(DOWN, deltaTime, isSprinting);

	}
}