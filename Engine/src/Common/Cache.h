#pragma once

#include "Hash.h"

namespace Engine
{
    template<typename T>
    class Cache
    {
    public:
        template<typename... Args>
        T& Get(Args&&... args)
        {
            std::size_t hash{ 0 };

            Hash(hash, args...);

            auto it = resources.find(hash);

            if (it != resources.end())
            {
                return *it->second;
            }

            auto resource = std::make_unique<T>(std::forward<Args>(args)...);

            auto inserted = resources.emplace(hash, std::move(resource));

            return *inserted.first->second;
        }

        void Clear()
        {
            resources.clear();
        }

    private:
        std::unordered_map<std::size_t, std::unique_ptr<T>> resources;
    };
}