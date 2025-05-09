#pragma once

#include "Scene.h"
#include "SceneResource.h"

namespace Engine
{
	class SceneGraph : public Scene
	{
	public:
		SceneGraph();

		Entity::Id Instantiate(std::shared_ptr<SceneResource> scene);
		void Replace(const SceneResource& scene);

		void Update();

		void Pause();
		void Resume();
		[[nodiscard]] bool IsPaused() const;

	private:
		void CopyEntityFromSceneInstance(Entity::Id entity, const Component::SceneInstance& instance);
		template<typename... T>
		void AddOverridesToSceneInstance(Entity::Id entity, Component::SceneInstance& instance, Component::GroupT<T...>) const;

		void ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity);

		bool paused = false;
	};
};
