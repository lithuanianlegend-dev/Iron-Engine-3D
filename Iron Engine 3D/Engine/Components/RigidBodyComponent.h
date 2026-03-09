#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

struct RigidBodyComponent
{
	JPH::BodyID bodyID; // this is JOLTS handle to a body id. wowaweewa.
	bool		isStatic = false;
	bool		isKinematic = false;
	float		mass = 1.f;
	float		restitution = 0.2f; // bouncy
	float		friction = 0.5f;
};