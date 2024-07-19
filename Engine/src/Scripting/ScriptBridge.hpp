#pragma once

#include "ScriptRunner.hpp"

#include <sol/sol.hpp>

namespace Engine
{
    class Entity;
    class Scene;

    class ScriptBridge
    {

    public:
        static void Register(ScriptRunner& runner);

    private:
        ScriptBridge() = default;
    };


}