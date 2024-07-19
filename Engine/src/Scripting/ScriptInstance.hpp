#pragma once

#include "Scene/Entity.hpp"

#include <sol/sol.hpp>

#include <memory>


namespace Engine
{
	class Script;

	class ScriptInstance
	{
	public:
		ScriptInstance(sol::state& lua, Script& script, Entity entity);

		void Start();
		void Update(float delta);

		sol::environment& GetEnv();

	private:
		sol::environment env;

		sol::function startFn;
		sol::function updateFn;

		Entity entity;
	};
}