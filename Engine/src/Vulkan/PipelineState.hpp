#pragma once

#include <vulkan/vulkan.h>

#include "PipelineLayout.hpp"
#include "RenderPass.hpp"

#include <vector>

namespace Vulkan
{
	struct VertexInputState
	{
		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;

		bool operator==(VertexInputState const&) const = default;
	};

	struct MultisampleState
	{
		VkSampleCountFlagBits rasterizationSamples{ VK_SAMPLE_COUNT_1_BIT };

		bool operator==(MultisampleState const&) const = default;
	};

	struct InputAssemblyState
	{
		VkPrimitiveTopology topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };

		bool operator==(InputAssemblyState const&) const = default;
	};

	struct RasterizationState
	{
		VkCullModeFlags cullMode{ VK_CULL_MODE_BACK_BIT };
		VkFrontFace frontFace{ VK_FRONT_FACE_COUNTER_CLOCKWISE };

		bool operator==(RasterizationState const&) const = default;
	};

	struct DepthStencilState
	{
		VkBool32 depthTestEnable{ VK_TRUE };
		VkBool32 depthWriteEnable{ VK_TRUE };

		bool operator==(DepthStencilState const&) const = default;
	};

	class PipelineState
	{
	public:
		void SetRenderPass(const RenderPass& renderPass);
		void SetPipelineLayout(PipelineLayout& pipelineLayout);

		void SetVertexInputState(const VertexInputState& state);
		void SetMultisampleState(const MultisampleState& state);
		void SetInputAssemblyState(const InputAssemblyState& state);
		void SetRasterizationState(const RasterizationState& state);
		void SetDepthStencilState(const DepthStencilState& state);

		void SetSubpassIndex(uint32_t index);

		void ClearDirty();
		void Reset();

		const RenderPass* GetRenderPass() const;
		const PipelineLayout* GetPipelineLayout() const;
		
		const VertexInputState& GetVertexInputState() const;
		const MultisampleState& GetMultisampleState() const;
		const InputAssemblyState& GetInputAssemblyState() const;
		const RasterizationState& GetRasterizationState() const;
		const DepthStencilState& GetDepthStencilState() const;

		uint32_t GetSubpassIndex() const;

		bool IsDirty() const;

	private:
		bool dirty{ false };

		const RenderPass* renderPass{ nullptr };
		PipelineLayout* pipelineLayout { nullptr };

		VertexInputState vertexInput{};
		MultisampleState multisample{};
		InputAssemblyState inputAssembly{};
		RasterizationState rasterization{};
		DepthStencilState depthStencil{};

		uint32_t subpassIndex{ 0 };
	};
}

namespace std
{
	template <>
	struct hash<Vulkan::PipelineState>
	{
		size_t operator()(const Vulkan::PipelineState& state) const
		{
			size_t hash{ 0 };

			HashCombine(hash, state.GetRenderPass());
			HashCombine(hash, state.GetPipelineLayout());

			return hash;
		}
	};
};