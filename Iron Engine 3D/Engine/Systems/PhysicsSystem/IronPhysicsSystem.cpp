// EVERYTHING PHYSICS RELATED HERE:
#include "IronPhysicsSystem.h"
#include "../../Components/TransformComponent.h"
#include "../../Components/RigidBodyComponent.h"

// JOLT PHYSICS ENGINE

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>

#include <iostream>
#include <gtc/matrix_transform.hpp>

#include <cstdarg>



JPH_SUPPRESS_WARNING_PUSH
JPH_CLANG_SUPPRESS_WARNING("-Wformat-nonliteral")
static void TraceImpl(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}
JPH_SUPPRESS_WARNING_POP

static bool AssertFailedImpl(const char* expr, const char* msg, const char* file, JPH::uint line) {
	std::cerr << "[Jolt] " << file << ":" << line << " (" << expr << ") " << (msg ? msg : "") << "\n";
	return true;
}



// Jolt boiler plate layers
namespace Layers
{
	static constexpr JPH::ObjectLayer STATIC = 0; // Object layers. We only want to really perform physic calculations on dynamic objects.
	static constexpr JPH::ObjectLayer DYNAMIC = 1;
}

class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface // final so NOTHING derives from BPlayerInterfaceImpl
{
	JPH::BroadPhaseLayer layers[2];
public:
	BPLayerInterfaceImpl()
	{
		layers[Layers::STATIC] = JPH::BroadPhaseLayer(0); // BroadPhaseLayer as if there are like a bajilion objects, we'd have to check against like a bajilion ^2 collision calculations 
		layers[Layers::DYNAMIC] = JPH::BroadPhaseLayer(1); // So BroadPhaseLayer just checks all the nearby collisions and if they are close enough!
	}
	uint32_t GetNumBroadPhaseLayers() const override { return 2; }
	JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer l) const override { return layers[l]; }
};

class OVBPLayerPairFilter : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	bool ShouldCollide(JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer layer) const override
	{
		if (objectLayer == Layers::DYNAMIC)
			return true; 
		if (objectLayer == Layers::STATIC)
			return false; 
		return false;
	}
};

class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	bool ShouldCollide(JPH::ObjectLayer a, JPH::ObjectLayer b) const override
	{
		if (a == Layers::STATIC)  return b == Layers::DYNAMIC;
		if (a == Layers::DYNAMIC) return true;
		return false;
	}
};

// -------------------------
// STATISTICS
// -------------------------
static JPH::PhysicsSystem*		  s_physicsSystem = nullptr;
static JPH::TempAllocatorImpl*	  s_allocator	 = nullptr;
static JPH::JobSystemThreadPool*  s_jobSystem	 = nullptr;
static BPLayerInterfaceImpl*	  s_bpLayer		 = nullptr;
static OVBPLayerPairFilter*		  s_ovbpFilter	 = nullptr;
static ObjectLayerPairFilterImpl* s_olFilter     = nullptr;

// NOW we can start the real physics
namespace PhysicsSystem
{
	void Init()
	{
		
		JPH::Trace = TraceImpl;
		JPH::AssertFailed = AssertFailedImpl;

		JPH::RegisterDefaultAllocator();
		JPH::Factory::sInstance = new JPH::Factory;
		JPH::RegisterTypes();

		s_allocator     = new JPH::TempAllocatorImpl(10 * 1024 * 1024); // 10MB
		s_jobSystem     = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs,
											       JPH::cMaxPhysicsBarriers, 2);
		s_bpLayer       = new BPLayerInterfaceImpl();
		s_ovbpFilter    = new OVBPLayerPairFilter();
		s_olFilter      = new ObjectLayerPairFilterImpl();

		s_physicsSystem = new JPH::PhysicsSystem();
		s_physicsSystem->Init(1024, 0, 1024, 1024,
							  *s_bpLayer, *s_ovbpFilter, *s_olFilter);

		s_physicsSystem->SetGravity(JPH::Vec3(0.f, -9.81f, 0.f));

		std::cout << "[PHYSICS SYSTEM] Initialized\n";
	}

	void RegisterBody(entt::registry& registry, entt::entity entity, bool isStatic)
	{
		auto& t = registry.get<TransformComponent>(entity);
		auto& rb = registry.get<RigidBodyComponent>(entity);

		// Box shape from entity scale
		JPH::BoxShapeSettings shapeSettings(
			JPH::Vec3(t.scale.x * 0.5f, t.scale.y * 0.5f, t.scale.z * 0.5f));
		auto shapeResult = shapeSettings.Create();
		if (shapeResult.HasError())
		{
			std::cout << "[PHYSICS SYSTEM] Shape error: " << shapeResult.GetError() << "\n";
			return;
		}

		JPH::BodyCreationSettings bodySettings(
			shapeResult.Get(),
			JPH::RVec3(t.position.x, t.position.y, t.position.z),
			JPH::Quat::sIdentity(),
			isStatic ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
			isStatic ? Layers::STATIC : Layers::DYNAMIC);

		bodySettings.mRestitution = rb.restitution;
		bodySettings.mFriction    = rb.friction;

		if (!isStatic)
			bodySettings.mMassPropertiesOverride.mMass = rb.mass; // if its not static, yay it has mass!

		auto& bodyInterface = s_physicsSystem->GetBodyInterface();
		rb.bodyID = bodyInterface.CreateAndAddBody(bodySettings,
			JPH::EActivation::Activate);
		rb.isStatic = isStatic;

		if (rb.bodyID.IsInvalid())
			std::cerr << "[Physics System] Failed to create body for entity\n";

	}

	void Step(entt::registry& registry, float deltaTime)
	{
		// Advances the simulation by deltaTime.
		// collisionSteps = 1 is fine for 60fps. and uhh Increase if you get tunneling at high speeds/
		s_physicsSystem->Update(deltaTime, 2, s_allocator, s_jobSystem);
			
		// Write Jolt body posiitons back into TransformComponents so the renderer sees shit
		auto& bodyInterface = s_physicsSystem->GetBodyInterface();
		auto view = registry.view<TransformComponent, RigidBodyComponent>();

		for (auto [entity, t, rb] : view.each())
		{
			if (rb.bodyID.IsInvalid() || rb.isStatic) continue; // static never ever duper ever moves so skip EVERYTHING.

			JPH::RVec3 pos = bodyInterface.GetPosition(rb.bodyID);
			JPH::Quat  rot = bodyInterface.GetRotation(rb.bodyID);

			t.position = { pos.GetX(), pos.GetY(), pos.GetZ() };

			// Convert Jolt quaternion to euler angles for transform component
			float sinX  = 2.f * (rot.GetW() * rot.GetX() + rot.GetY() * rot.GetZ());
			float cosX  = 1.f - 2.f * (rot.GetX() * rot.GetX() + rot.GetY() * rot.GetY());
			float sinY  = 2.f * (rot.GetW() * rot.GetY() - rot.GetZ() * rot.GetX());
			float sinZ  = 2.f * (rot.GetW() * rot.GetZ() + rot.GetX() * rot.GetY());
			float cosZ = 1.f - 2.f * (rot.GetY() * rot.GetY() + rot.GetZ() * rot.GetZ());

			t.rotation.x = glm::degrees(atan2f(sinX, cosX));
			t.rotation.y = glm::degrees(asinf(glm::clamp(sinY, -1.f, 1.f)));
			t.rotation.z = glm::degrees(atan2f(sinZ, cosZ));

			// now a very few of you may be wondering, what the fuck just happened?
			// Id like to ask that too. Im literally just following formulas.
		

			// Rebuild worldMatrix so rendering system picks up the new transform 
			glm::mat4 m = glm::translate(glm::mat4(1.f), t.position);
			m = glm::rotate(m, glm::radians(t.rotation.x), { 1, 0, 0 });
			m = glm::rotate(m, glm::radians(t.rotation.y), { 0, 1, 0 });
			m = glm::rotate(m, glm::radians(t.rotation.z), { 0, 0, 1 });
			m = glm::scale(m, t.scale);
			t.worldMatrix = m;
		}
	}

	void UnregisterBody(entt::registry& registry, entt::entity entity)
	{
		auto& rb = registry.get<RigidBodyComponent>(entity);
		if (rb.bodyID.IsInvalid())
		{
			std::cerr << "[PHYSICS SYSTEM] Entity does not contain RigidBody. UnregisterBody() will NOT be performed.\n";
			return;
		}

		auto& bodyInterface = s_physicsSystem->GetBodyInterface();
		bodyInterface.RemoveBody(rb.bodyID);  // remove from simulation
		bodyInterface.DestroyBody(rb.bodyID); // free memory
		rb.bodyID = JPH::BodyID();			  // reset to invalid
	}

	void Shutdown()
	{
		delete s_physicsSystem; s_physicsSystem = nullptr;
		delete s_bpLayer;		s_bpLayer	    = nullptr;
		delete s_ovbpFilter;	s_ovbpFilter	= nullptr;
		delete s_olFilter;		s_olFilter		= nullptr;
		delete s_jobSystem;		s_jobSystem		= nullptr;
		delete s_allocator;	    s_allocator		= nullptr;
	
		JPH::UnregisterTypes();
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;

		std::cout << "[Physics System] Shutdown successfull\n";
	}
}