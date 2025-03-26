#pragma once

#include "Scene.h"
#include "SceneResource.h"

namespace Engine
{
	class SceneGraph : public Scene
	{
	public:
		SceneGraph();

		void Add(const SceneResource& scene);

		std::unique_ptr<SceneResource> Pack() const;

		void Update();

		void Pause();
		void Resume();
		[[nodiscard]] bool IsPaused() const;

	private:
		void ComputeEntityLocalToWorld(const Component::LocalToWorld& parent, Entity::Id entity);

		bool paused = false;
	};
};
