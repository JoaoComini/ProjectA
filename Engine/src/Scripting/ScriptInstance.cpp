#include "ScriptInstance.hpp"

#include "Script.hpp"
#include "ScriptBridge.hpp"

namespace Engine
{
	ScriptInstance::ScriptInstance(sol::state& lua, Script& script, Entity entity)
		: env(lua, sol::create, lua.globals()), entity(entity)
	{
		env["entity"] = entity;

		auto result = lua.script(script.GetCode(), env);

		if (! result.valid())
		{
			sol::error err = result;
			std::cerr << "failed to create script: " << err.what() << std::endl;
			return;
		}

		startFn = env["start"];
		updateFn = env["update"];
	}

	void ScriptInstance::Start()
	{
		if (startFn == sol::nil)
		{
			return;
		}

		auto result = startFn();

		if (result.valid())
		{
			return;
		}

		sol::error err = result;
		std::cerr << "failed to start script: " << err.what() << std::endl;
	}

	void ScriptInstance::Update(float delta)
	{
		if (updateFn == sol::nil)
		{
			return;
		}

		auto result = updateFn(delta);

		if (result.valid())
		{
			return;
		}

		sol::error err = result;
		std::cerr << "failed to update script: " << err.what() << std::endl;
	}

	sol::environment& ScriptInstance::GetEnv()
	{
		return env;
	}
}
