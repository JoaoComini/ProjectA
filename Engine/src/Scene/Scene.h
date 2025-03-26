#pragma once

#include "SceneMixin.h"
#include "Entity.h"
#include "Components.h"

// template<typename Type, typename EnTT>
// struct entt::storage_type<Type, EnTT> {
//     using type = Engine::SceneMixin<basic_storage<Type, EnTT>>;
// };

namespace Engine
{
    template<typename...>
    struct ExclusionT {
        explicit constexpr ExclusionT() = default;
    };

    template<typename... Types>
    inline constexpr ExclusionT<Types...> Exclusion{};

    template<typename, typename>
    struct SceneQuery;

    template<typename... Get, typename... Exclude>
    struct SceneQuery<entt::type_list<Get...>, entt::type_list<Exclude...>>
    {
        using Iterator = typename entt::view<entt::get_t<Get...>, entt::exclude_t<Exclude...>>::iterator;

        explicit SceneQuery(entt::registry& registry)
        {
            view = registry.view<Get...>(entt::exclude<Exclude...>);
        }

        template<typename T>
        decltype(auto) TryGetComponent(Entity::Id id) const
        {
            return view.template try_get<T>(id);
        }

        template<typename T>
        [[nodiscard]] bool HasComponent(Entity::Id id) const
        {
            return view.template any_of<T>(id);
        }

        template<typename... T>
        decltype(auto) GetComponent(Entity::Id id) const
        {
            return view.get<T...>(id);
        }

        [[nodiscard]] Entity::Id First() const
        {
            return view.front();
        }

        Iterator begin() const noexcept {
            return view.begin();
        }

        Iterator end() const noexcept {
            return view.end();
        }

    private:
        entt::view<entt::get_t<Get...>, entt::exclude_t<Exclude...>> view;
    };

    class Scene
    {
    public:
        Entity::Id CreateEntity();
        void DestroyEntity(Entity::Id entity);

        [[nodiscard]] bool Valid(Entity::Id entity) const;

        void SetParent(Entity::Id entity, Entity::Id parent);
        Entity::Id GetParent(Entity::Id entity);

        template<typename... Get, typename... Exclude>
        decltype(auto) Query(ExclusionT<Exclude...> = Exclusion<Exclude...>)
        {
            return SceneQuery<entt::type_list<Get...>, entt::type_list<Component::Delete, Exclude...>>(registry);
        }

        template<typename... Args>
        void Clear()
        {
            registry.clear<Args...>();
        }

        template<typename T, typename... Args>
        decltype(auto) AddComponent(Entity::Id entity, Args&&... args)
        {
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        decltype(auto) AddOrReplaceComponent(Entity::Id entity, Args&&... args)
        {
            return registry.emplace_or_replace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) GetComponent(Entity::Id id)
        {
            return registry.get<T...>(id);
        }

        template<typename T, typename... Args>
        decltype(auto) GetOrAddComponent(Entity::Id id, Args&&... args)
        {
            return registry.get_or_emplace<T>(id, std::forward<Args>(args)...);
        }

        template<typename... T>
        auto TryGetComponent(Entity::Id id)
        {
            return registry.try_get<T...>(id);
        }

        template<typename T>
        [[nodiscard]] bool HasComponent(Entity::Id id) const
        {
            return registry.any_of<T>(id);
        }

        template<typename... T>
        void RemoveComponent(Entity::Id id)
        {
            registry.remove<T...>(id);
        }

        template<typename T, auto FreeFunc>
        void OnComponentAdded()
        {
            registry.on_construct<T>().template connect<FreeFunc>();
        }

        template<typename T, auto MemberFunc, typename Type>
        void OnComponentAdded(Type instance)
        {
            registry.on_construct<T>().template connect<MemberFunc>(instance);
        }

        template<class Archive>
        static void SaveSnapshot(Archive& ar, const Scene& scene)
        {
            entt::snapshot snapshot{ scene.registry };
            snapshot.get<entt::entity>(ar);

            SnapshotComponentGroup(ar, snapshot, Component::Serializable);
        }

        template<class Archive>
        static void LoadSnapshot(Archive& ar, Scene& scene)
        {
            entt::snapshot_loader snapshot{ scene.registry };
            snapshot.get<entt::entity>(ar);

            SnapshotComponentGroup(ar, snapshot, Component::Serializable);

            snapshot.orphans();
        }

        template<class Archive, class Snapshot, typename... Components>
        static void SnapshotComponentGroup(Archive& ar, Snapshot& snapshot, Component::Group<Components...>)
        {
            ([&]()
            {
                snapshot.template get<Components>(ar);
            }(), ...);
        }

    protected:
        void AddChild(Entity::Id parent, Entity::Id child);
        void RemoveChild(Entity::Id parent, Entity::Id child);

        entt::registry registry;

        friend class SceneGraph;
        friend class SceneResource;
    };
}

