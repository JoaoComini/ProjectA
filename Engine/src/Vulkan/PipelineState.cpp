#include "PipelineState.hpp"

bool operator==(const VkVertexInputAttributeDescription& lhs, const VkVertexInputAttributeDescription& rhs)
{
    return std::tie(lhs.binding, lhs.format, lhs.location, lhs.offset) == std::tie(rhs.binding, rhs.format, rhs.location, rhs.offset);
}

bool operator==(const VkVertexInputBindingDescription& lhs, const VkVertexInputBindingDescription& rhs)
{
    return std::tie(lhs.binding, lhs.inputRate, lhs.stride) == std::tie(rhs.binding, rhs.inputRate, rhs.stride);
}

namespace Vulkan
{
    void PipelineState::SetRenderPass(const RenderPass& renderPass)
    {
        if (!this->renderPass || this->renderPass->GetHandle() != renderPass.GetHandle())
        {
            this->renderPass = &renderPass;

            dirty = true;
        }
    }

    void PipelineState::SetPipelineLayout(PipelineLayout& pipelineLayout)
    {
        if (!this->pipelineLayout || this->pipelineLayout->GetHandle() != pipelineLayout.GetHandle())
        {
            this->pipelineLayout = &pipelineLayout;

            dirty = true;
        }
    }

    void PipelineState::SetVertexInputState(const VertexInputState& state)
    {
        if (vertexInput != state)
        {
            vertexInput = state;

            dirty = true;
        }
    }

    void PipelineState::SetMultisampleState(const MultisampleState& state)
    {
        if (multisample != state)
        {
            multisample = state;

            dirty = true;
        }
    }

    void PipelineState::SetInputAssemblyState(const InputAssemblyState& state)
    {
        if (inputAssembly != state)
        {
            inputAssembly = state;

            dirty = true;
        }
    }

    void PipelineState::SetRasterizationState(const RasterizationState& state)
    {
        if (rasterization != state)
        {
            rasterization = state;

            dirty = true;
        }
    }

    void PipelineState::SetDepthStencilState(const DepthStencilState& state)
    {
        if (depthStencil != state)
        {
            depthStencil = state;

            dirty = true;
        }
    }

    void PipelineState::SetSubpassIndex(uint32_t index)
    {
        if (subpassIndex != index)
        {
            subpassIndex = index;

            dirty = true;
        }
    }

    void PipelineState::ClearDirty()
    {
        dirty = false;
    }

    void PipelineState::Reset()
    {
        ClearDirty();

        renderPass = nullptr;
        pipelineLayout = nullptr;

        vertexInput.attributes.clear();
        vertexInput.bindings.clear();

        multisample = {};
        inputAssembly = {};
        rasterization = {};
        depthStencil = {};
        subpassIndex = 0;
    }

    const RenderPass* PipelineState::GetRenderPass() const
    {
        return renderPass;
    }

    const PipelineLayout* PipelineState::GetPipelineLayout() const
    {
        return pipelineLayout;
    }

    const VertexInputState& PipelineState::GetVertexInputState() const
    {
        return vertexInput;
    }

    const MultisampleState& PipelineState::GetMultisampleState() const
    {
        return multisample;
    }

    const InputAssemblyState& PipelineState::GetInputAssemblyState() const
    {
        return inputAssembly;
    }

    const RasterizationState& PipelineState::GetRasterizationState() const
    {
        return rasterization;
    }

    const DepthStencilState& PipelineState::GetDepthStencilState() const
    {
        return depthStencil;
    }

    uint32_t PipelineState::GetSubpassIndex() const
    {
        return subpassIndex;
    }

    bool PipelineState::IsDirty() const
    {
        return dirty;
    }
}