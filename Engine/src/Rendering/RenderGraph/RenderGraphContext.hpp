#pragma once

#include <typeindex>
#include <any>

namespace Engine
{
    class RenderGraphContext
    {
    public:
        template<typename T, typename... Args>
        decltype(auto) Add(Args&& ...args)
        {
            assert(!Has<T>());

            return storage[typeid(T)].emplace<T>(T{ std::forward<Args>(args)... });
        }

        template<typename T>
        T& Get()
        {
            assert(Has<T>());

            return std::any_cast<T&>(storage.at(typeid(T)));
        }

        template <typename T>
        bool Has() const {
            return storage.contains(typeid(T));
        }

    private:
        std::unordered_map<std::type_index, std::any> storage;
    };
}