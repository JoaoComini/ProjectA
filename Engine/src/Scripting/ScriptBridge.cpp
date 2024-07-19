#include "ScriptBridge.hpp"

#include "Core/Input.hpp"

#include "Scene/Components.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"

#include "ScriptRunner.hpp"

namespace Engine
{
    void RegisterEntity(ScriptRunner& runner)
    {
        auto lua = runner.GetState();

        auto type = lua.new_usertype<Entity>("Entity", sol::no_constructor);

        type["empty"] = sol::factories([]() -> Entity { return {}; });

        type["transform"] = sol::property(&Entity::TryGetComponent<Component::Transform>);
        type["add_transform"] = &Entity::AddComponent<Component::Transform>;

        type["name"] = sol::property(&Entity::GetName, &Entity::SetName);
        type["parent"] = sol::property(&Entity::GetParent, &Entity::SetParent);

        type["script"] = sol::property([&runner](const Entity& self) -> sol::object {
            if (auto instance = runner.FindScriptInstance(self))
            {
                return instance->GetEnv();
            }

            return sol::nil;
        });
    }

    void RegisterComponents(ScriptRunner& runner)
    {
        auto lua = runner.GetState();

        auto type = lua.new_usertype<Component::Transform>("Transform");
        type["position"] = &Component::Transform::position;
    }

    void RegisterVec3(ScriptRunner& runner)
    {
        auto lua = runner.GetState();

        auto type = lua.new_usertype<glm::vec3>(
            "Vec3",
            sol::constructors<glm::vec3(float), glm::vec3(float, float, float)>()
        );

        type["x"] = &glm::vec3::x;
        type["y"] = &glm::vec3::y;
        type["z"] = &glm::vec3::z;

        type[sol::meta_function::addition] = sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(&glm::operator+);
        type[sol::meta_function::subtraction] = sol::resolve<glm::vec3(const glm::vec3&, const glm::vec3&)>(&glm::operator-);
        type[sol::meta_function::unary_minus] = sol::resolve<glm::vec3(const glm::vec3&)>(&glm::operator-);
        type[sol::meta_function::multiplication] = sol::resolve<glm::vec3(const glm::vec3&, const float)>(&glm::operator*);

        type["length"] = sol::resolve<float(const glm::vec3&)>(&glm::length);
        type["normalize"] = sol::resolve<glm::vec3(const glm::vec3&)>(&glm::normalize);
    }

    void RegisterInput(ScriptRunner& runner)
    {
        auto lua = runner.GetState();

        lua.new_enum<KeyCode>("KeyCode", {
            { "Space", KeyCode::Space },
            { "A", KeyCode::A },
            { "B", KeyCode::B },
            { "C", KeyCode::C },
            { "D", KeyCode::D },
            { "E", KeyCode::E },
            { "F", KeyCode::F },
            { "G", KeyCode::G },
            { "H", KeyCode::H },
            { "I", KeyCode::I },
            { "J", KeyCode::J },
            { "K", KeyCode::K },
            { "L", KeyCode::L },
            { "M", KeyCode::N },
            { "O", KeyCode::O },
            { "P", KeyCode::P },
            { "Q", KeyCode::Q },
            { "R", KeyCode::R },
            { "S", KeyCode::S },
            { "T", KeyCode::T },
            { "U", KeyCode::U },
            { "V", KeyCode::V },
            { "W", KeyCode::W },
            { "X", KeyCode::X },
            { "Y", KeyCode::Y },
            { "Z", KeyCode::Z },
        });

        lua.new_enum<MouseCode>("MouseCode", {
            { "Button0", MouseCode::Button0},
            { "Button1", MouseCode::Button1},
            { "Button2", MouseCode::Button2},
            { "Button3", MouseCode::Button3},
            { "Button4", MouseCode::Button4},
            { "Button5", MouseCode::Button5},
            { "Button6", MouseCode::Button6},
            { "Button7", MouseCode::Button7},
            { "ButtonLeft", MouseCode::ButtonLeft},
            { "ButtonMiddle", MouseCode::ButtonMiddle},
            { "ButtonRight", MouseCode::ButtonRight},
        });

        lua.set_function("is_key_down", [](KeyCode code) -> bool {
            return Input::Get().IsKeyDown(code);
        });

        lua.set_function("is_mouse_button_down", [](MouseCode code) -> bool {
            return Input::Get().IsMouseButtonDown(code);
        });
    }

    void RegisterScene(ScriptRunner& runner)
    {
        auto lua = runner.GetState();
        auto& scene = runner.GetScene();

        auto type = lua.new_usertype<Scene>("Scene", sol::no_constructor);

        lua.set_function("find_entity_by_name", [&scene](sol::this_state state, const std::string& name) -> sol::object {
            auto entity = scene.FindFirstEntity<Component::Name>([&name](auto entity) {
                return entity.GetName() == name;
            });

            if (entity)
            {
                return sol::object(state, sol::in_place, entity);
            }

            return sol::nil;
        });

        lua.set_function("create_entity", [&scene]() -> Entity {
            return scene.CreateEntity();
        });

        lua.set_function("add_scene", [&scene](const Scene& other) -> void {
            return scene.Add(other);
        });
    }

    void ScriptBridge::Register(ScriptRunner& runner)
    {
        RegisterEntity(runner);
        RegisterComponents(runner);
        RegisterVec3(runner);
        RegisterInput(runner);
        RegisterScene(runner);
    }
}