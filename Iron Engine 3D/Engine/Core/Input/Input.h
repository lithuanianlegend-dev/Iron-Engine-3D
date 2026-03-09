#pragma once

#include <glad.h>
#include <glfw3.h>

#include "../Engine/Engine.h"

namespace IronEngine
{

	class InputHandler
	{
	public:
		InputHandler(const InputHandler&) = delete;
		InputHandler& operator = (const InputHandler&) = delete;

		static InputHandler& Ref()
		{
			static InputHandler reference;
			return reference;
		}

		void Poll(GLFWwindow* window, entt::registry& registry, entt::entity camera);
		
	private:
		InputHandler();
	};

	static InputHandler& Input = InputHandler::Ref();
}