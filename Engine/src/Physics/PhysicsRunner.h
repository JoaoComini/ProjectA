#pragma once

#include <Jolt/Jolt.h>

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "JoltBroadPhaseLayer.h"
#include "JoltContactListener.h"
#include "JoltLayerMapper.h"

#include "Scene/SceneGraph.h"

namespace Engine
{
	using namespace std::literals;

	namespace Component
	{
		struct PhysicsBody;
	};

	class PhysicsRunner
	{
		using Duration = std::chrono::duration<float>;

	public:
		PhysicsRunner(SceneGraph& scene);

		void Start();
		void Update(float timestep);
		void Stop();

	private:

		void CreateAndAddBody(JPH::ShapeRefC shape, Entity::Id entity);
		void CreateAndAddBody(JPH::ShapeRefC shape, Entity::Id entity, Entity::Id parent);

		void PrepareStaticBody(JPH::BodyID id, Entity::Id entity);
		void PrepareKinematicBody(JPH::BodyID id, Component::PhysicsBody& body);

		void UpdateDynamicBody(JPH::BodyID id, Entity::Id entity);

		JoltLayerMapper layerMapper;

		std::unique_ptr<JoltContactListener> contactListener;
		std::unique_ptr<JPH::TempAllocatorImpl> allocator;
		std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;

		std::unique_ptr<JPH::PhysicsSystem> system;

		Duration timestep = 1s / 60.f;
		Duration accumulator = 0s;

		float alpha = 0;

		SceneGraph& scene;
	};
}
