// EVERYTHING THAT HAS TO DO WITH PHYSICS, GOES HERE.
#pragma once
#include <entt/entt.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

namespace PhysicsSystem
{
	void Init();
	void RegisterBody(entt::registry& registry, entt::entity entity, bool isStatic);
	void Step(entt::registry& registry, float deltaTime);
	void Shutdown();
}