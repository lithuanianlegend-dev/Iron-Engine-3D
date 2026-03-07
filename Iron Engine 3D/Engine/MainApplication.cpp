#include <iostream>

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <entt/entt.hpp>

// CORE
#include "Core/Engine/Engine.h"
#include "Core/Input/Input.h"
#include "Core/Events/EventHandler.h"
#include "Core/Time/Time.h"

#include "Core/Scenes/Scene.h"

int main()
{
	IronEngine::Core.InitializeEngine(1200, 800, "Iron Engine 3D");


	auto& registry = IronEngine::Core.GetRegistry();
	Scene scene;
	scene.Init(registry);


	while (IronEngine::Core.IsRunning())
	{
		scene.Update(registry);

		IronEngine::Core.Update();
		IronEngine::Core.Render();

		IronEngine::Input.Poll(&IronEngine::Core.GetWindow());

		IronEngine::EventHandler.PollEvents();

		IronEngine::Time.Tick();
		IronEngine::Time.MeasureFrameTime(&IronEngine::Core.GetWindow());

		IronEngine::Core.RefreshEngine();
	}

	scene.Destroy(registry);
	return EXIT_SUCCESS;
}