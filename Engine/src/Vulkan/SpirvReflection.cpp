#include "SpirvReflection.hpp"


namespace Vulkan
{

    SpirvReflection::SpirvReflection(std::vector<uint32_t>& spirv, ShaderStage stage)
        : compiler(spirv), stage(stage)
    {
    }

    void Vulkan::SpirvReflection::ReflectShaderResources(std::vector<ShaderResource>& shaderResources)
    {
        ParseShaderResource<ShaderResourceType::Input>(shaderResources);
        ParseShaderResource<ShaderResourceType::Output>(shaderResources);
        ParseShaderResource<ShaderResourceType::BufferUniform>(shaderResources);
        ParseShaderResource<ShaderResourceType::ImageSampler>(shaderResources);
    }

    template<>
    void SpirvReflection::ParseShaderResource<ShaderResourceType::Input>(std::vector<ShaderResource>& shaderResources)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        for (auto& input : resources.stage_inputs)
        {
            ShaderResource resource = CreateShaderResource<ShaderResourceType::Input>(input);

            ParseResourceVecSize(input, resource);
            ParseResourceArraySize(input, resource);

            ParseResourceDecoration<spv::DecorationLocation>(input, resource);

            shaderResources.push_back(resource);
        }
    }

    template<>
    void SpirvReflection::ParseShaderResource<ShaderResourceType::Output>(std::vector<ShaderResource>& shaderResources)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        for (auto& output : resources.stage_outputs)
        {
            ShaderResource resource = CreateShaderResource<ShaderResourceType::Output>(output);

            ParseResourceVecSize(output, resource);
            ParseResourceArraySize(output, resource);

            ParseResourceDecoration<spv::DecorationLocation>(output, resource);

            shaderResources.push_back(resource);
        }
    }

    template<>
    void SpirvReflection::ParseShaderResource<ShaderResourceType::BufferUniform>(std::vector<ShaderResource>& shaderResources)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        for (auto& uniform : resources.uniform_buffers)
        {
            ShaderResource resource = CreateShaderResource<ShaderResourceType::BufferUniform>(uniform);

            ParseResourceArraySize(uniform, resource);
            ParseResourceSize(uniform, resource);

            ParseResourceDecoration<spv::DecorationDescriptorSet>(uniform, resource);
            ParseResourceDecoration<spv::DecorationBinding>(uniform, resource);

            shaderResources.push_back(resource);
        }
    }

    template<>
    void SpirvReflection::ParseShaderResource<ShaderResourceType::ImageSampler>(std::vector<ShaderResource>& shaderResources)
    {
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        for (auto& image : resources.sampled_images)
        {
            ShaderResource resource = CreateShaderResource<ShaderResourceType::ImageSampler>(image);

            ParseResourceArraySize(image, resource);

            ParseResourceDecoration<spv::DecorationDescriptorSet>(image, resource);
            ParseResourceDecoration<spv::DecorationBinding>(image, resource);

            shaderResources.push_back(resource);
        }
    }

    void SpirvReflection::ParseResourceVecSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        const auto& type = compiler.get_type_from_variable(resource.id);

        shaderResource.vecSize = type.vecsize;
        shaderResource.columns = type.columns;
    }

    void SpirvReflection::ParseResourceArraySize(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        const auto& type = compiler.get_type_from_variable(resource.id);

        shaderResource.arraySize = type.array.size() ? type.array[0] : 1;
    }

    void SpirvReflection::ParseResourceSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        const auto& type = compiler.get_type_from_variable(resource.id);
        shaderResource.size = static_cast<uint32_t>(compiler.get_declared_struct_size(type));
    }

    template<>
    void SpirvReflection::ParseResourceDecoration<spv::DecorationLocation>(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        shaderResource.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
    }

    template<>
    void SpirvReflection::ParseResourceDecoration<spv::DecorationDescriptorSet>(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        shaderResource.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    }

    template<>
    void SpirvReflection::ParseResourceDecoration<spv::DecorationBinding>(const spirv_cross::Resource& resource, ShaderResource& shaderResource)
    {
        shaderResource.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
    }
};