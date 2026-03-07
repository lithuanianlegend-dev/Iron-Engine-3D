#include "EventHandler.h"

namespace IronEngine
{
	Event::Event()
	{
		
	}

	void Event::PollEvents()
	{
		glfwPollEvents();
	}
}