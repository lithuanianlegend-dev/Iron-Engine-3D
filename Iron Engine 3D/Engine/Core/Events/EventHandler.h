#pragma once

#include <glad.h>
#include <glfw3.h>

namespace IronEngine
{

	class Event
	{
	public:
		Event(const Event&) = delete;
		Event& operator = (const Event&) = delete;

		static Event& Ref()
		{
			static Event reference;
			return reference;
		}

		void PollEvents();

	private:
		Event();
	};

	static Event& EventHandler = Event::Ref();
}