#pragma once
#include <glad.h>
#include <glfw3.h>
#include "../Engine/Rendering/Camera/Camera.h"
#include "../Engine/Core/Engine/Engine.h"
#include "../Engine/Components/CameraComponent.h"

class Mouse
{
public:
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};

struct WindowState
{
    entt::registry* registry = nullptr;
    entt::entity cameraEntity;
    Mouse* mouse;
    float lastX = 400.0f;
    float lastY = 300.0f;
    bool firstMouse = true;
    IronEngine::Engine* engine;
};

// inline means to copy everything inside a function. only for small scripts like these
inline void Mouse::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // woah this is weird. basically, it creates from our previous struc and it just has all our things nicely made
    // so we dont have to make them ourselves. how nice am I?

    // glfwGetWindowUserPointer(window) GLFW basically lets u attach any object to a window
    // with glfwSetWindowUserPointer() but as you can see this just retrieves it. however, GLFW doesnt know
    // what type it is so it returns void*.
    WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); // static_cast basically says i know this is a windowstate so treat it like it or smth
    if (!state) return;
   
    auto& registry = *state->registry;
    
    auto& transform = registry.get<TransformComponent>(state->cameraEntity);
    auto& camera = registry.get<CameraComponent>(state->cameraEntity);


    if (state->firstMouse)
    {
        state->lastX = xpos;
        state->lastY = ypos;
        state->firstMouse = false;
    }
    float xoffset = (float)(xpos - state->lastX);
    float yoffset = (float)(state->lastY - ypos);
    state->lastX = (float)xpos;
    state->lastY = (float)ypos;

    camera.engineCamera.ProcessMouseMovement(xoffset, yoffset);
}

void Mouse::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (!state || !state->registry) return;

    auto& registry = *state->registry;
    auto& camera = registry.get<CameraComponent>(state->cameraEntity);

    camera.engineCamera.ProcessMouseScroll((float)yoffset);
}