#pragma once

#include <glad.h>
#include <glfw3.h>
#include <entt/entt.hpp>

namespace IronEngine 
{
	
	class Engine
	{
	public:
		Engine(const Engine&) = delete;
		Engine& operator = (const Engine&) = delete;
		~Engine();

		static Engine& Ref()
		{
			static Engine reference;
			return reference;
		}

		void InitializeEngine(int width, int height, const char* title);
		void RefreshEngine();
		void DestroyEngine();

		void UpdateProjection();

		void Update();
		void Render();

		inline bool			   IsRunning()					 { return m_isRunning; }
		inline GLFWwindow&	   GetWindow() const			 { return *m_window; }
		inline float		   WindowWidth()				 { return m_width; }
		inline float		   WindowHeight()				 { return m_height;  }
		inline entt::registry& GetRegistry()				 { return registry; }
		inline void			   SetWidth(int w)				 { m_width = w; }
		inline void			   SetHeight(int h)				 { m_height = h; }
		inline void			   SetCurrentShader(uint32_t id) { m_currentShaderID = id; }
	private:
		Engine();
	private:
		uint32_t m_currentShaderID = 0;

		int m_width, m_height;
		bool m_isRunning;
		GLFWwindow* m_window;

		entt::registry registry;
	};

	static Engine& Core = Engine::Ref();
}