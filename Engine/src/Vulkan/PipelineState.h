#pragma once

#include "PipelineLayout.h"

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
		VkBool32 depthClampEnable{ VK_FALSE };

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

	struct ColorBlendAttachmentState
	{
		VkBool32 blendEnable{ VK_FALSE };

		bool operator==(ColorBlendAttachmentState const&) const = default;
	};

	struct ColorBlendState
	{
		std::vector<ColorBlendAttachmentState> attachments;

		bool operator==(ColorBlendState const&) const = default;
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
		void SetColorBlendState(const ColorBlendState& state);

		void ClearDirty();
		void Reset();

		const PipelineLayout* GetPipelineLayout() const;
		
		const VertexInputState& GetVertexInputState() const;
		const MultisampleState& GetMultisampleState() const;
		const InputAssemblyState& GetInputAssemblyState() const;
		const RasterizationState& GetRasterizationState() const;
		const DepthStencilState& GetDepthStencilState() const;
		const PipelineRenderingState& GetPipelineRenderingState() const;
		const ColorBlendState& GetColorBlendState() const;

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
		ColorBlendState colorBlend{};
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
			HashCombine(hash, state.GetColorBlendState());

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
			HashCombine(hash, state.depthClampEnable);

			return hash;
		}
	};

	template <>
	struct hash<Vulkan::ColorBlendAttachmentState>
	{
		size_t operator()(const Vulkan::ColorBlendAttachmentState& state) const
		{
			size_t hash{ 0 };

			HashCombine(hash, state.blendEnable);

			return hash;
		}
	};

	template <>
	struct hash<Vulkan::ColorBlendState>
	{
		size_t operator()(const Vulkan::ColorBlendState& state) const
		{
			size_t hash{ 0 };

			HashCombine(hash, state.attachments);

			return hash;
		}
	};

};