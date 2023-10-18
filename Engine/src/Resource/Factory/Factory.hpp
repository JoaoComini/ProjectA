#pragma once

#include "../Resource.hpp"

#include <filesystem>
#include <memory>

namespace Engine
{
    template<typename T, typename P>
    class Factory
    {
    public:
        virtual ~Factory() {}

        virtual void Create(std::filesystem::path destination, P& payload) = 0;

        virtual std::shared_ptr<T> Load(std::filesystem::path source) = 0;
    };
};