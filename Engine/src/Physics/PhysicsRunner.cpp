#include "PhysicsRunner.hpp"

#include "Scene/Scene.hpp"
#include "Scene/TransformHelper.hpp"

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
		JPH::RegisterDefaultAllocator();

		JPH::Factory::sInstance = new JPH::Factory();

		JPH::RegisterTypes();

		contactListener = std::make_unique<JoltContactListener>(scene, *system);
		allocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);
		jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, 1);
	}

	struct ShapeConfig
	{
		JPH::Shape* shape;
		JPH::Vec3 position;
		JPH::Quat rotation;
	};

	JPH::ShapeRefC BuildCompoundShape(const std::vector<ShapeConfig>& configs)
	{
		JPH::StaticCompoundShapeSettings settings;

		for (auto& config : configs)
		{
			settings.AddShape(
				config.position,
				config.rotation,
				config.shape
			);
		}

		auto result = settings.Create();

		if (!result.IsValid())
		{
			return nullptr;
		}

		return result.Get();
	}

	void GetShapes(Entity entity, glm::mat4 parent, std::vector<ShapeConfig>& configs)
	{
		auto& transform = entity.GetComponent<Component::Transform>();

		glm::vec3 position = parent * glm::vec4{ transform.position, 1 };
		glm::quat rotation = parent * glm::mat4_cast(transform.rotation);

		if (auto box = entity.TryGetComponent<Component::BoxShape>())
		{
			configs.push_back({
				.shape = new JPH::BoxShape(ToJolt(box->size)),
				.position = ToJolt(position + box->offset),
				.rotation = ToJolt(rotation)
			});
		}

		if (auto sphere = entity.TryGetComponent<Component::SphereShape>())
		{
			configs.push_back({
				.shape = new JPH::SphereShape(sphere->radius),
				.position = ToJolt(position + sphere->offset),
				.rotation = ToJolt(rotation)
			});
		}

		if (auto children = entity.TryGetComponent<Component::Children>())
		{
			auto current = children->first;

			auto matrix = TransformHelper::ComputeEntityWorldMatrix(entity);

			for (size_t i = 0; i < children->size; i++)
			{
				GetShapes(current, matrix, configs);

				current = current.GetComponent<Component::Hierarchy>().next;
			}
		}
	}

	JPH::ShapeRefC BuildShapeForEntity(Entity entity)
	{
		std::vector<ShapeConfig> configs;

		GetShapes(entity, glm::mat4{ 1.f }, configs);

		return configs.size() == 1
			? configs[0].shape
			: BuildCompoundShape(configs);
	}

	void PhysicsRunner::Start()
	{
		system = std::make_unique<JPH::PhysicsSystem>();

		const uint32_t maxBodies = 65536;
		const uint32_t numBodyMutexes = 0;
		const uint32_t maxBodyPairs = 65536;
		const uint32_t maxContactConstraints = 10240;

		system->Init(maxBodies, numBodyMutexes, maxBodyPairs, maxContactConstraints, layerMapper, layerMapper, layerMapper);

		system->SetContactListener(contactListener.get());

		scene.ForEachEntity<Component::PhysicsBody>([&](auto entity) {
			auto shape = BuildShapeForEntity(entity);

			CreateAndAddBody(shape, entity);
		});

		system->OptimizeBroadPhase();
	}

	void PhysicsRunner::CreateAndAddBody(JPH::ShapeRefC shape, Entity entity)
	{
		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		auto& body = entity.GetComponent<Component::PhysicsBody>();
		auto& transform = entity.GetComponent<Component::Transform>();

		auto position = transform.position;
		auto rotation = transform.rotation;

		if (auto hierarchy = entity.TryGetComponent<Component::Hierarchy>())
		{
			auto matrix = TransformHelper::ComputeEntityWorldMatrix(hierarchy->parent);

			position = matrix * glm::vec4{ position, 1.f };
			rotation = matrix * glm::mat4_cast(rotation);
		}

		JPH::BodyCreationSettings bodySettings(
			shape,
			ToJolt(position),
			ToJolt(rotation),
			ToJolt(body.type),
			body.type == Component::PhysicsBody::MotionType::Static
				? Layer::NON_MOVING
				: Layer::MOVING
		);

		bodySettings.mUserData = entity.GetHandle();
		
		body.id = bodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate).GetIndexAndSequenceNumber();
	}

	void PhysicsRunner::Update(float timestep)
	{
		accumulator += Duration(timestep);

		scene.Clear<Component::PhysicsContactEnter>();
		scene.Clear<Component::PhysicsContactExit>();

		scene.ForEachEntity<Component::PhysicsBody>([&](Entity entity) {
			auto& body = entity.GetComponent<Component::PhysicsBody>();

			auto id = JPH::BodyID(body.id);

			if (id.IsInvalid())
			{
				return;
			}

			if (body.type == Component::PhysicsBody::MotionType::Dynamic)
			{
				return;
			}

			switch (body.type)
			{
			case Component::PhysicsBody::MotionType::Static:
				PrepareStaticBody(id, entity);
				break;
			case Component::PhysicsBody::MotionType::Kinematic:
				PrepareKinematicBody(id, body);
				break;
			}
		});

		while (accumulator >= this->timestep)
		{
			accumulator -= this->timestep;

			system->Update(this->timestep.count(), 1, allocator.get(), jobSystem.get());
		}
		
		scene.ForEachEntity<Component::PhysicsBody>([&](Entity entity) {
			auto& body = entity.GetComponent<Component::PhysicsBody>();

			auto id = JPH::BodyID(body.id);

			if (id.IsInvalid())
			{
				return;
			}

			if (body.type == Component::PhysicsBody::MotionType::Static)
			{
				return;
			}

			UpdateDynamicBody(id, entity);
		});

		alpha = accumulator / this->timestep;
	}

	void PhysicsRunner::PrepareStaticBody(JPH::BodyID id, Entity entity)
	{
		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		auto& transform = entity.GetComponent<Component::Transform>();

		auto position = transform.position;
		auto rotation = transform.rotation;

		if (auto hierarchy = entity.TryGetComponent<Component::Hierarchy>())
		{
			auto matrix = TransformHelper::ComputeEntityWorldMatrix(hierarchy->parent);

			position = matrix * glm::vec4{ position, 1.f };
			rotation = matrix * glm::mat4_cast(rotation);
		}

		bodyInterface.SetPositionAndRotation(
			id,
			ToJolt(position),
			ToJolt(rotation),
			JPH::EActivation::DontActivate
		);
	}

	void PhysicsRunner::PrepareKinematicBody(JPH::BodyID id, Component::PhysicsBody& body)
	{
		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		bodyInterface.SetLinearVelocity(id, ToJolt(body.velocity));
	}

	void PhysicsRunner::UpdateDynamicBody(JPH::BodyID id, Entity entity)
	{
		JPH::BodyInterface& bodyInterface = system->GetBodyInterface();

		auto position = ToGlm(bodyInterface.GetPosition(id));
		auto rotation = ToGlm(bodyInterface.GetRotation(id));

		if (auto hierarchy = entity.TryGetComponent<Component::Hierarchy>())
		{
			auto matrix = TransformHelper::ComputeEntityWorldMatrix(hierarchy->parent);

			auto inverse = glm::inverse(matrix);
			position = inverse * glm::vec4{ position, 1.f };
			rotation = inverse * glm::mat4_cast(rotation);
		}

		auto& transform = entity.GetComponent<Component::Transform>();

		transform.position = position;
		transform.rotation = rotation;
	}

	void PhysicsRunner::Stop()
	{
		system = nullptr;
	}
}
