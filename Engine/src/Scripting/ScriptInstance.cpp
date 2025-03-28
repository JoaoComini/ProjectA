#include "ScriptInstance.h"

#include "Script.h"
#include "ScriptBridge.h"

namespace Engine
{
	ScriptInstance::ScriptInstance(sol::state& lua, Script& script, ScriptEntity entity)
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
		onContactEnterFn = env["on_contact_enter"];
		onContactExitFn = env["on_contact_exit"];
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

	void ScriptInstance::OnContactEnter(ScriptEntity other)
	{
		if (onContactEnterFn == sol::nil)
		{
			return;
		}

		auto result = onContactEnterFn(other);

		if (result.valid())
		{
			return;
		}

		sol::error err = result;
		std::cerr << "failed to send on_contact: " << err.what() << std::endl;
	}

	void ScriptInstance::OnContactExit(ScriptEntity other)
	{
		if (onContactExitFn == sol::nil)
		{
			return;
		}

		auto result = onContactExitFn(other);

		if (result.valid())
		{
			return;
		}

		sol::error err = result;
		std::cerr << "failed to send on_contact: " << err.what() << std::endl;
	}

	sol::environment& ScriptInstance::GetEnv()
	{
		return env;
	}
}
