#pragma once

#include "ShaderModule.hpp"

namespace Vulkan
{
    class GlslCompiler
    {
    public:
        void CompileToSpv(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant, std::vector<uint32_t>& spirv);
    };

}