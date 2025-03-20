#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Engine
{
	class Scene;

	class JoltContactListener final : public JPH::ContactListener
	{
	public:
		JoltContactListener(Scene& scene, JPH::PhysicsSystem& system);

		virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;
		virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

	private:
		JPH::PhysicsSystem& system;
		Scene& scene;
	};
}

