#pragma once

#include "ScriptEntity.h"

#include <sol/sol.hpp>

namespace Engine
{
	class Script;
	class ScriptEntity;

	class ScriptInstance
	{
	public:
		ScriptInstance(sol::state& lua, Script& script, ScriptEntity entity);

		void Start();
		void Update(float delta);
		void OnContactEnter(ScriptEntity other);
		void OnContactExit(ScriptEntity other);

		sol::environment& GetEnv();

	private:
		sol::environment env;

		sol::function startFn;
		sol::function updateFn;
		sol::function onContactEnterFn;
		sol::function onContactExitFn;

		ScriptEntity entity;
	};
}