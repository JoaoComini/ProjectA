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

    void PipelineState::SetPipelineRenderingState(const PipelineRenderingState& state)
    {
        if (pipelineRendering != state)
        {
            pipelineRendering = state;

            dirty = true;
        }
    }

    void PipelineState::SetColorBlendState(const ColorBlendState& state)
    {
        if (colorBlend != state)
        {
            colorBlend = state;

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

        pipelineLayout = nullptr;

        vertexInput.attributes.clear();
        vertexInput.bindings.clear();

        multisample = {};
        inputAssembly = {};
        rasterization = {};
        depthStencil = {};
        pipelineRendering = {};
        colorBlend = {};
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

    const PipelineRenderingState& PipelineState::GetPipelineRenderingState() const
    {
        return pipelineRendering;
    }

    const ColorBlendState& PipelineState::GetColorBlendState() const
    {
        return colorBlend;
    }

    bool PipelineState::IsDirty() const
    {
        return dirty;
    }
}