#pragma once

#include "ScriptInstance.hpp"

#include <sol/sol.hpp>

namespace Engine
{
	class Scene;

	class ScriptRunner
	{
	public:
		ScriptRunner(Scene& scene);

		void Start();
		void Update(float delta);
		void Stop();

		ScriptInstance* FindScriptInstance(Entity entity);

		Scene& GetScene() const;
		sol::state_view GetState() const;

	private:
		Scene& scene;
		sol::state lua;

		std::unordered_map<uint32_t, ScriptInstance> instances;
	};
}