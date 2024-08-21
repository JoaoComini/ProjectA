#include "GlslCompiler.hpp"

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

namespace Vulkan
{
    void GlslCompiler::CompileToSpv(VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant, std::vector<uint32_t>& spirv)
    {
        glslang::InitializeProcess();

        EShLanguage language = EShLanguage::EShLangVertex;

        switch (stage)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            language = EShLangVertex;
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            language = EShLangFragment;
            break;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            language = EShLangGeometry;
            break;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            language = EShLangCompute;
            break;
        };

        EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

        auto src = source.GetSource().c_str();

        glslang::TShader shader(language);
        shader.setStrings(&src, 1);
        shader.setPreamble(variant.GetPreamble().c_str());

        if (!shader.parse(GetDefaultResources(), 100, false, messages))
        {
            throw std::runtime_error(std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog()));
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages))
        {
            throw std::runtime_error(std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog()));
        }

        glslang::TIntermediate* intermediate = program.getIntermediate(language);

        if (!intermediate)
        {
            throw std::runtime_error("failed to get shared intermediate code");
        }

        spv::SpvBuildLogger logger;

        glslang::GlslangToSpv(*intermediate, spirv, &logger);

        glslang::FinalizeProcess();
    }
}
