#include "Time.h"
#include <iostream>
#include <string>

namespace IronEngine
{
	
	T_Time::T_Time() : deltaTime(0.0f), lastFrame(0.0f), curTime(0.0f), prevTime(0.0f) {}

	void T_Time::Tick()
	{
		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();
	}

	void T_Time::MeasureFrameTime(GLFWwindow* window)
	{
		curTime = glfwGetTime();
		timeDiff = curTime - prevTime;
		counter++;
		if (timeDiff >= 0.1f)
		{
			int FPS = (1.0f / timeDiff) * counter;
			int ms = (timeDiff / counter) * 1000;
			std::string newTitle = "Iron Engine - " + std::to_string(FPS) + "FPS / " + std::to_string(ms) + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			prevTime = curTime;
			counter = 0;
		}
	}
}