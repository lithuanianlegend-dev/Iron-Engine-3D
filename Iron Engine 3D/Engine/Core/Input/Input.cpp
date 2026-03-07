#include "Input.h"

namespace IronEngine
{
	InputHandler::InputHandler() {  }

	void InputHandler::Poll(GLFWwindow* window)
	{

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			Core.DestroyEngine();

	}
}