#include "ScriptRunner.hpp"

#include "Script.hpp"

#include "Scene/Scene.hpp"
#include "Resource/ResourceManager.hpp"

#include "ScriptBridge.hpp"

namespace Engine
{
	ScriptRunner::ScriptRunner(Scene& scene) : scene(scene)
	{
		lua.open_libraries(sol::lib::base);

		ScriptBridge::Register(*this);
	}

	void ScriptRunner::Start()
	{
		scene.ForEachEntity<Component::Script>([&](auto entity) {
			auto id = entity.GetComponent<Component::Script>().script;

			if (!id)
			{
				return;
			}

			auto script = ResourceManager::Get().LoadResource<Script>(id);

			if (!script)
			{
				return;
			}

			auto instance = ScriptInstance(lua, *script, entity);

			instances.emplace(entity.GetHandle(), std::move(instance));
		});

		for (auto& [_, instance] : instances)
		{
			instance.Start();
		}
	}

	void ScriptRunner::Update(float delta)
	{
		scene.ForEachEntity<Component::Script>([&](auto entity) {
			if (auto instance = FindScriptInstance(entity))
			{
				instance->Update(delta);
			}
		});
	}

	void ScriptRunner::Stop()
	{
		instances.clear();
	}

	ScriptInstance* ScriptRunner::FindScriptInstance(Entity entity)
	{
		if (auto it = instances.find(entity.GetHandle()); it != instances.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	Scene& ScriptRunner::GetScene() const
	{
		return scene;
	}

	sol::state_view ScriptRunner::GetState() const
	{
		return lua;
	}
}