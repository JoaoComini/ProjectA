#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "JoltBroadPhaseLayer.hpp"
#include "JoltContactListener.hpp"
#include "JoltLayerMapper.hpp"

namespace Engine
{
	using namespace std::literals;

	class Scene;
	class Entity;

	namespace Component
	{
		struct PhysicsBody;
	};

	class PhysicsRunner
	{
		using Duration = std::chrono::duration<float>;

	public:
		PhysicsRunner(Scene& scene);

		void Start();
		void Update(float timestep);
		void Stop();

	private:

		void CreateAndAddBody(JPH::ShapeRefC shape, Entity entity);
		void CreateAndAddBody(JPH::ShapeRefC shape, Entity entity, Entity parent);

		void PrepareStaticBody(JPH::BodyID id, Entity entity);
		void PrepareKinematicBody(JPH::BodyID id, Component::PhysicsBody& body);

		void UpdateDynamicBody(JPH::BodyID id, Entity entity);

		JoltLayerMapper layerMapper;

		std::unique_ptr<JoltContactListener> contactListener;
		std::unique_ptr<JPH::TempAllocatorImpl> allocator;
		std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

		std::unique_ptr<JPH::PhysicsSystem> system;

		Duration timestep = 1s / 60.f;
		Duration accumulator = 0s;

		float alpha = 0;

		Scene& scene;
	};
}