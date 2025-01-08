#pragma once

#include "PipelineLayout.hpp"

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

	struct PipelineRenderingState
	{
		std::vector<VkFormat> colorAttachmentFormats;
		VkFormat depthAttachmentFormat;

		bool operator==(PipelineRenderingState const&) const = default;
	};

	class PipelineState
	{
	public:
		void SetPipelineLayout(PipelineLayout& pipelineLayout);

		void SetVertexInputState(const VertexInputState& state);
		void SetMultisampleState(const MultisampleState& state);
		void SetInputAssemblyState(const InputAssemblyState& state);
		void SetRasterizationState(const RasterizationState& state);
		void SetDepthStencilState(const DepthStencilState& state);
		void SetPipelineRenderingState(const PipelineRenderingState& state);

		void SetSubpassIndex(uint32_t index);

		void ClearDirty();
		void Reset();

		const PipelineLayout* GetPipelineLayout() const;
		
		const VertexInputState& GetVertexInputState() const;
		const MultisampleState& GetMultisampleState() const;
		const InputAssemblyState& GetInputAssemblyState() const;
		const RasterizationState& GetRasterizationState() const;
		const DepthStencilState& GetDepthStencilState() const;
		const PipelineRenderingState& GetPipelineRenderingState() const;

		uint32_t GetSubpassIndex() const;

		bool IsDirty() const;

	private:
		bool dirty{ false };

		PipelineLayout* pipelineLayout { nullptr };

		VertexInputState vertexInput{};
		MultisampleState multisample{};
		InputAssemblyState inputAssembly{};
		RasterizationState rasterization{};
		DepthStencilState depthStencil{};
		PipelineRenderingState pipelineRendering{};

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

			HashCombine(hash, state.GetPipelineLayout());
			HashCombine(hash, state.GetPipelineRenderingState());
			HashCombine(hash, state.GetRasterizationState());

			return hash;
		}
	};

	template <>
	struct hash<Vulkan::PipelineRenderingState>
	{
		size_t operator()(const Vulkan::PipelineRenderingState& state) const
		{
			size_t hash{ 0 };
			
			HashCombine(hash, state.colorAttachmentFormats);
			HashCombine(hash, state.depthAttachmentFormat);

			return hash;
		}
	};

	template <>
	struct hash<Vulkan::RasterizationState>
	{
		size_t operator()(const Vulkan::RasterizationState& state) const
		{
			size_t hash{ 0 };

			HashCombine(hash, state.cullMode);
			HashCombine(hash, state.frontFace);

			return hash;
		}
	};

};