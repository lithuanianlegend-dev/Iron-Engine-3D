#include "Engine.h"
#include <iostream>

#include "../Engine/Systems/RenderSystem.h"
#include "../Engine/Systems/TransformSystem.h"



namespace IronEngine
{

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);

		Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
		if (engine) {
			engine->SetWidth(width);
			engine->SetHeight(height);
			engine->UpdateProjection();
		}
	}

	Engine::Engine() : m_width(0), m_height(0), m_window(nullptr), m_isRunning(false) {}

	void Engine::InitializeEngine(int width, int height, const char* title)
	{
		m_width = width;
		m_height = height;
		m_isRunning = true;
		
		if (!glfwInit())
		{
			std::cout << "GLFW: Failed to initialize!" << std::endl;
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		auto& monitor = *glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwWindowHint(GLFW_RED_BITS, monitor.redBits);
		glfwWindowHint(GLFW_BLUE_BITS, monitor.blueBits);
		glfwWindowHint(GLFW_GREEN_BITS, monitor.greenBits);

		m_window = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!m_window)
		{
			std::cout << "GLFW: Failed to create window!" << std::endl;
			return;
		}

		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

		glfwMakeContextCurrent(m_window);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "FAILED TO INITIALIZE GLAD" << std::endl;
			return;
		}

		glfwSwapInterval(1);
	}

	void Engine::Update()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		TransformSystem::Update(registry);
	}

	void Engine::Render()
	{
		RenderSystem::Render(registry);
	}

	void Engine::RefreshEngine()
	{
		glfwSwapBuffers(m_window);
		glfwPollEvents();

		if (glfwWindowShouldClose(m_window))
			m_isRunning = false;
	}

	void Engine::UpdateProjection()
	{
		if (m_currentShaderID == 0) {
			std::cout << "ERROR: Shader not set for UpdateProjection()!" << std::endl;
			return;
		}
		float aspect = (float)m_width / (float)m_height;
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		ShaderLibrary::SetMat4(m_currentShaderID, "u_Projection", projection);
	}

	void Engine::DestroyEngine()
	{
		if (m_window)
			glfwDestroyWindow(m_window);

		glfwTerminate();
		m_window = nullptr;
		m_isRunning = false;
	}

	Engine::~Engine()
	{
		DestroyEngine();
	}

}