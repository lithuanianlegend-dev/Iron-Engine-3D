#pragma once

#include <glfw3.h>

namespace IronEngine
{
	
	class T_Time
	{
	public:
		T_Time(const T_Time&) = delete;
		T_Time& operator = (const T_Time&) = delete;

		static T_Time& Ref()
		{
			static T_Time reference;
			return reference;
		}

		void Tick();
		void MeasureFrameTime(GLFWwindow* window);
		inline float GetDeltaTime() { return deltaTime; }

	private:
		T_Time();

	private:

		float deltaTime, lastFrame;
		double curTime, prevTime;

		double timeDiff;

		unsigned int counter;
	};

	static T_Time& Time = T_Time::Ref();
}