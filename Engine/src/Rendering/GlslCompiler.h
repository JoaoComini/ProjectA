#pragma once

#include "Shader.h"

namespace Engine
{
    class GlslCompiler
    {
    public:
        void CompileToSpv(ShaderStage stage, const ShaderSource& source, const ShaderVariant& variant, std::vector<uint32_t>& spirv);
    };

}