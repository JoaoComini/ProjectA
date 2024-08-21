#pragma once

#include <entt/entt.hpp>

namespace Engine
{
    class Entity;

    template<typename Type>
    class SceneMixin final : public Type {
        using underlying_type = Type;
        using basic_registry_type = entt::basic_registry<typename underlying_type::entity_type, typename underlying_type::base_type::allocator_type>;
        using underlying_iterator = typename underlying_type::base_type::basic_iterator;

        using sigh_type = entt::sigh<void(Entity), typename underlying_type::allocator_type>;

        basic_registry_type& owner_or_assert() const noexcept {
            ENTT_ASSERT(owner != nullptr, "Invalid pointer to registry");
            return *owner;
        }

        void pop(underlying_iterator first, underlying_iterator last) final {
            if (auto& reg = owner_or_assert(); destruction.empty()) {
                underlying_type::pop(first, last);
            }
            else {
                for (; first != last; ++first) {
                    const auto entt = *first;
                    destruction.publish({entt, &reg});
                    const auto it = underlying_type::find(entt);
                    underlying_type::pop(it, it + 1u);
                }
            }
        }

        void pop_all() final {
            if (auto& reg = owner_or_assert(); !destruction.empty()) {
                for (auto pos = underlying_type::each().begin().base().index(); !(pos < 0); --pos) {
                    if constexpr (underlying_type::traits_type::in_place_delete) {
                        if (const auto entt = underlying_type::operator[](static_cast<typename underlying_type::size_type>(pos)); entt != entt::tombstone) {
                            destruction.publish({ entt, &reg });
                        }
                    }
                    else {
                        destruction.publish({ underlying_type::operator[](static_cast<typename underlying_type::size_type>(pos)), &reg });
                    }
                }
            }

            underlying_type::pop_all();
        }

        underlying_iterator try_emplace(const typename underlying_type::entity_type entt, const bool force_back, const void* value) final {
            const auto it = underlying_type::try_emplace(entt, force_back, value);

            if (auto& reg = owner_or_assert(); it != underlying_type::base_type::end()) {
                construction.publish({ *it, &reg });
            }

            return it;
        }

    public:
        using allocator_type = typename underlying_type::allocator_type;
        using entity_type = typename underlying_type::entity_type;
        using registry_type = basic_registry_type;

        SceneMixin()
            : SceneMixin{ allocator_type{} } {}

        explicit SceneMixin(const allocator_type& allocator)
            : underlying_type{ allocator },
            owner{},
            construction{ allocator },
            destruction{ allocator },
            update{ allocator } {}

        SceneMixin(SceneMixin&& other) noexcept
            : underlying_type{ std::move(other) },
            owner{ other.owner },
            construction{ std::move(other.construction) },
            destruction{ std::move(other.destruction) },
            update{ std::move(other.update) } {}

        SceneMixin(SceneMixin&& other, const allocator_type& allocator) noexcept
            : underlying_type{ std::move(other), allocator },
            owner{ other.owner },
            construction{ std::move(other.construction), allocator },
            destruction{ std::move(other.destruction), allocator },
            update{ std::move(other.update), allocator } {}

        SceneMixin& operator=(SceneMixin&& other) noexcept {
            underlying_type::operator=(std::move(other));
            owner = other.owner;
            construction = std::move(other.construction);
            destruction = std::move(other.destruction);
            update = std::move(other.update);
            return *this;
        }

        void swap(SceneMixin& other) {
            using std::swap;
            underlying_type::swap(other);
            swap(owner, other.owner);
            swap(construction, other.construction);
            swap(destruction, other.destruction);
            swap(update, other.update);
        }

        [[nodiscard]] auto on_construct() noexcept {
            return entt::sink{ construction };
        }

        [[nodiscard]] auto on_update() noexcept {
            return entt::sink{ update };
        }

        [[nodiscard]] auto on_destroy() noexcept {
            return entt::sink{ destruction };
        }

        auto emplace() {
            const auto entt = underlying_type::emplace();
            construction.publish({ entt, &owner_or_assert() });
            return entt;
        }

        template<typename... Args>
        decltype(auto) emplace(const entity_type hint, Args &&...args) {
            if constexpr (std::is_same_v<typename underlying_type::value_type, typename underlying_type::entity_type>) {
                const auto entt = underlying_type::emplace(hint, std::forward<Args>(args)...);
                construction.publish({ entt, &owner_or_assert() });
                return entt;
            }
            else {
                underlying_type::emplace(hint, std::forward<Args>(args)...);
                construction.publish({ hint, &owner_or_assert() });
                return this->get(hint);
            }
        }

        template<typename... Func>
        decltype(auto) patch(const entity_type entt, Func &&...func) {
            underlying_type::patch(entt, std::forward<Func>(func)...);
            update.publish({ entt, &owner_or_assert() });
            return this->get(entt);
        }

        template<typename It, typename... Args>
        void insert(It first, It last, Args &&...args) {
            underlying_type::insert(first, last, std::forward<Args>(args)...);

            if (auto& reg = owner_or_assert(); !construction.empty()) {
                for (; first != last; ++first) {
                    construction.publish({ *first, &owner_or_assert() });
                }
            }
        }

        void bind(entt::any value) noexcept final {
            auto* reg = any_cast<basic_registry_type>(&value);
            owner = reg ? reg : owner;
            underlying_type::bind(std::move(value));
        }

    private:
        basic_registry_type* owner;
        sigh_type construction;
        sigh_type destruction;
        sigh_type update;
    };
};

template<typename Type, typename Entt>
struct entt::storage_type<Type, Entt> {
    using type = Engine::SceneMixin<entt::basic_storage<Type, Entt>>;
};
