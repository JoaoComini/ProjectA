#pragma once

#include "ScriptRunner.hpp"

#include <sol/sol.hpp>

namespace Engine
{
    class ScriptBridge
    {

    public:
        static void Register(ScriptRunner& runner);

    private:
        ScriptBridge() = default;
    };


}