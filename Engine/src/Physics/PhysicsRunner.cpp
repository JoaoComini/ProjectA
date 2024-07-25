#include "PhysicsRunner.hpp"

#include "Scene/Scene.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace Engine
{
	JPH::EMotionType ToJolt(Component::PhysicsBody::MotionType type)
	{
		switch (type)
		{
		case Engine::Component::PhysicsBody::MotionType::Static:
			return JPH::EMotionType::Static;
		case Engine::Component::PhysicsBody::MotionType::Kinematic:
			return JPH::EMotionType::Kinematic;
		case Engine::Component::PhysicsBody::MotionType::Dynamic:
			return JPH::EMotionType::Dynamic;
		}

		return JPH::EMotionType::Static;
	}

	JPH::Vec3 ToJolt(const glm::vec3& vec)
	{
		return JPH::Vec3(vec.x, vec.y, vec.z);
	}

	JPH::Quat ToJolt(const glm::quat& quat)
	{
		return JPH::Quat(quat.x, quat.y, quat.z, quat.w);
	}

	glm::vec3 ToGlm(const JPH::Vec3& vec)
	{
		return { vec.GetX(), vec.GetY(), vec.GetZ() };
	}

	glm::quat ToGlm(const JPH::Quat& quat)
	{
		return { quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ() };
	}

	PhysicsRunner::PhysicsRunner(Scene& scene) : scene(scene)
	{
		// Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
		// This needs to be done before any other Jolt function is called.
		JPH::RegisterDefaultAllocator();


		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// It is not directly used in this example but still required.
		JPH::Factory::sInstance = new JPH::Factory();

		// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
		// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
		// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
		JPH::RegisterTypes();

		temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
		job_system = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 1);
	}

	void PhysicsRunner::Start()
	{
		system = std::make_unique<JPH::PhysicsSystem>();
		// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
		const uint32_t maxBodies = 65536;

		// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
		const uint32_t numBodyMutexes = 0;

		// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
		// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
		// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
		const uint32_t maxBodyPairs = 65536;

		// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
		// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
		// Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
		const uint32_t maxContactConstraints = 10240;

		system->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		system->SetBodyActivationListener(&body_activation_listener);

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		system->SetContactListener(&contact_listener);

		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		scene.ForEachEntity<Component::PhysicsBody>([&bodyInterface](auto entity) {
			auto& transform = entity.GetComponent<Component::Transform>();
			auto& body = entity.GetComponent<Component::PhysicsBody>();

			auto position = transform.position;
			auto rotation = transform.rotation;

			JPH::StaticCompoundShapeSettings shapeSettings;

			if (auto box = entity.TryGetComponent<Component::BoxShape>())
			{
				shapeSettings.AddShape(
					ToJolt(box->offset),
					JPH::Quat::sIdentity(),
					new JPH::BoxShape(ToJolt(box->size))
				);
			}

			if (auto sphere = entity.TryGetComponent<Component::SphereShape>())
			{
				shapeSettings.AddShape(
					ToJolt(sphere->offset),
					JPH::Quat::sIdentity(),
					new JPH::SphereShape(sphere->radius)
				);
			}

			auto result = shapeSettings.Create();

			if (!result.IsValid())
			{
				return;
			}

			auto& shape = result.Get();

			JPH::BodyCreationSettings bodySettings(
				shape,
				ToJolt(position),
				ToJolt(rotation),
				ToJolt(body.type),
				Layers::MOVING
			);

			body.id = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate).GetIndexAndSequenceNumber();
		});

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		system->OptimizeBroadPhase();
	}

	void PhysicsRunner::Update(float timestep)
	{
		accumulator += Duration(timestep);

		while (accumulator >= this->timestep)
		{
			accumulator -= this->timestep;

			system->Update(this->timestep.count(), 1, temp_allocator.get(), job_system.get());
		}
		
		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		scene.ForEachEntity<Component::PhysicsBody>([&bodyInterface](auto entity) {
			auto& transform = entity.GetComponent<Component::Transform>();
			auto& body = entity.GetComponent<Component::PhysicsBody>();

			auto id = JPH::BodyID(body.id);

			if (id.IsInvalid())
			{
				return;
			}

			auto position = bodyInterface.GetPosition(id);
			auto rotation = bodyInterface.GetRotation(id);
			
			transform.position = ToGlm(position);
			transform.rotation = ToGlm(rotation);
		});

		alpha = accumulator / this->timestep;
	}

	void PhysicsRunner::Stop()
	{
		system = nullptr;
	}
}
