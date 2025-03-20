#include "ScriptBridge.h"

#include "Core/Input.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

#include "ScriptRunner.h"
#include "ScriptEntity.h"

namespace Engine
{
    void RegisterEntity(ScriptRunner& runner)
    {
        auto lua = runner.GetState();
        auto& scene = runner.GetScene();

        auto type = lua.new_usertype<ScriptEntity>("Entity", sol::no_constructor);

        type["transform"] = sol::property(&ScriptEntity::TryGetComponent<Component::Transform>);
        type["physics"] = sol::property(&ScriptEntity::TryGetComponent<Component::PhysicsBody>);

        type[sol::meta_function::index] = [&runner](const ScriptEntity& self, sol::stack_object key) -> sol::object {
            if (auto instance = runner.FindScriptInstance(self.GetId()))
            {
                auto& env = instance->GetEnv();

                return env[key];
            }

            return sol::nil;
        };

        type[sol::meta_function::new_index] = [&runner](const ScriptEntity& self, sol::stack_object key, sol::stack_object value) -> void {
            if (auto instance = runner.FindScriptInstance(self.GetId()))
            {
                auto& env = instance->GetEnv();

                env[key] = value;
            }
        };
    }

    void RegisterComponents(ScriptRunner& runner)
    {
        auto lua = runner.GetState();

        {
            auto type = lua.new_usertype<Component::Transform>("Transform");
            type["position"] = &Component::Transform::position;
        }

        {
            auto type = lua.new_usertype<Component::PhysicsBody>("PhysicsBody");
            type["velocity"] = &Component::PhysicsBody::velocity;
        }
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

        lua.new_enum<MouseButton>("MouseButton", {
            { "0", MouseButton::Left},
            { "1", MouseButton::Right},
            { "2", MouseButton::Middle},
            { "3", MouseButton::Back},
            { "4", MouseButton::Forward},
            { "Left", MouseButton::Left},
            { "Middle", MouseButton::Right},
            { "Right", MouseButton::Middle},
        });

        lua.set_function("is_key_down", [](KeyCode code) -> bool {
            return Input::Get().IsKeyDown(code);
        });

        lua.set_function("is_mouse_button_down", [](MouseButton code) -> bool {
            return Input::Get().IsMouseButtonDown(code);
        });
    }

    void RegisterScene(ScriptRunner& runner)
    {
        auto lua = runner.GetState();
        auto& scene = runner.GetScene();

        auto table = lua.create_named_table("scene");
           
        auto metatable = lua.create_table();

        metatable.set_function("find_entity_by_name", [&scene](sol::this_state state, const std::string& name) -> sol::object {
            auto query = scene.Query<Component::Name>();
            auto entity = query.First();

            if (scene.Valid(entity))
            {
                return sol::object(state, sol::in_place, entity);
            }

            return sol::nil;
        });

        metatable.set_function("create_entity", [&scene]() -> ScriptEntity {
            auto entity = scene.CreateEntity();

            return ScriptEntity(scene, entity);
        });

        metatable.set_function("destroy_entity", [&scene](const ScriptEntity& entity) -> void {
            scene.DestroyEntity(entity.GetId());
        });

        metatable.set_function("add", [&scene](const Scene& other) -> void {
            scene.Add(other);
        });

        metatable[sol::meta_function::new_index] = [](lua_State* state) {
            return luaL_error(state, "the scene can not be modified: use \"scene.add\" to add scenes");
        };

        metatable[sol::meta_function::index] = metatable;

        table[sol::metatable_key] = metatable;
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