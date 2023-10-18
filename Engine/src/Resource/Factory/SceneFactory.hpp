#pragma once

#include "Scene/Scene.hpp"

#include "Factory.hpp"


namespace Engine
{
	class SceneFactory : Factory<Scene, Scene>
	{
	public:
		void Create(std::filesystem::path destination, Scene& scene) override;
		std::shared_ptr<Scene> Load(std::filesystem::path source) override;
	};
};