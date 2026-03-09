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
	Scene scene;

	IronEngine::Core.InitializeEngine(1200, 800, "Iron Engine 3D");

	auto& registry = IronEngine::Core.GetRegistry();
	scene.Init(registry);

	IronEngine::Core.SetCamera(scene.GetCamera());

	while (IronEngine::Core.IsRunning())
	{
		IronEngine::Input.Poll(&IronEngine::Core.GetWindow(), IronEngine::Core.GetRegistry(), scene.GetCamera());
		IronEngine::EventHandler.PollEvents();

		IronEngine::Core.Update();

		scene.Update(registry);

		IronEngine::Core.Render();


		IronEngine::Time.Tick();
		IronEngine::Time.MeasureFrameTime(&IronEngine::Core.GetWindow());

		IronEngine::Core.RefreshEngine();
	}

	scene.Destroy(registry);
	return EXIT_SUCCESS;
}