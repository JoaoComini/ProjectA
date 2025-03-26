#include "ScriptRunner.h"

#include "Script.h"

#include "Resource/ResourceManager.h"

#include "ScriptBridge.h"

namespace Engine
{
	ScriptRunner::ScriptRunner(SceneGraph& scene) : scene(scene)
	{
		lua.open_libraries(sol::lib::base);

		ScriptBridge::Register(*this);
	}

	void ScriptRunner::Start()
	{
		auto query = scene.Query<Component::Script>();

		for (auto entity : query)
		{
			auto id = query.GetComponent<Component::Script>(entity).script;

			if (!id)
			{
				continue;
			}

			auto script = ResourceManager::Get().LoadResource<Script>(id);

			if (!script)
			{
				continue;
			}

			auto instance = ScriptInstance(lua, *script, {scene, entity});

			instances.emplace(static_cast<Entity::Id>(entity), std::move(instance));
		}

		for (auto& [_, instance] : instances)
		{
			instance.Start();
		}
	}

	void ScriptRunner::Update(float delta)
	{
		{
			auto query = scene.Query<Component::Script, Component::PhysicsContactEnter>();

			for (auto entity : query)
			{
				if (auto instance = FindScriptInstance(entity))
				{
					auto& contact = query.GetComponent<Component::PhysicsContactEnter>(entity);

					instance->OnContactEnter({ scene, contact.other });
				}
			}
		}

		{
			auto query = scene.Query<Component::Script, Component::PhysicsContactExit>();

			for (auto entity : query)
			{
				if (auto instance = FindScriptInstance(entity))
				{
					auto& contact = query.GetComponent<Component::PhysicsContactExit>(entity);

					instance->OnContactExit({ scene, contact.other });
				}
			}
		}

		{
			auto query = scene.Query<Component::Script>();

			for (auto entity : query)
			{
				if (auto instance = FindScriptInstance(entity))
				{
					instance->Update(delta);
				}
			}
		}
	}

	void ScriptRunner::Stop()
	{
		instances.clear();
	}

	ScriptInstance* ScriptRunner::FindScriptInstance(Entity::Id entity)
	{
		if (auto it = instances.find(entity); it != instances.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	SceneGraph& ScriptRunner::GetScene() const
	{
		return scene;
	}

	sol::state_view ScriptRunner::GetState() const
	{
		return lua;
	}
}