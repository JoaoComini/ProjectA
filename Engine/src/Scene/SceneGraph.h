#pragma once

#include "Scene.h"
#include "SceneResource.h"

namespace Engine
{
	class SceneGraph : public Scene
	{
	public:
		SceneGraph();

		[[nodiscard]] std::unique_ptr<SceneResource> Pack() const;

		Entity::Id Instantiate(std::shared_ptr<SceneResource> scene);
		void Replace(const SceneResource& scene);

		void Update();

		void Pause();
		void Resume();
		[[nodiscard]] bool IsPaused() const;

	private:
		void CopyEntityFromSceneInstance(Entity::Id entity, const Component::SceneInstance& from);
		void ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity);

		bool paused = false;
	};
};
