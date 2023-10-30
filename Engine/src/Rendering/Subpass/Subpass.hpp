#pragma once

#include "Rendering/Camera.hpp"
#include "Rendering/RenderFrame.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "Vulkan/Sampler.hpp"

#include <unordered_map>

namespace Engine
{
	class Subpass
	{
	public:
		Subpass(Vulkan::Device& device, Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource);
		virtual ~Subpass() = default;

		virtual void Prepare(Vulkan::RenderPass& renderPass);

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) = 0;

		const Vulkan::ShaderSource& GetVertexShader() const;
		const Vulkan::ShaderSource& GetFragmentShader() const;

		const std::vector<uint32_t>& GetInputAttachments() const;
		void SetInputAttachments(std::vector<uint32_t> input);

		const std::vector<uint32_t>& GetOutputAttachments() const;
		void SetOutputAttachments(std::vector<uint32_t> output);

		const std::vector<uint32_t>& GetColorResolveAttachments() const;
		void SetColorResolveAttachments(std::vector<uint32_t> colorResolve);

		bool IsDepthStencilDisabled() const;
		void EnableDepthStencil();

		void SetSampleCount(VkSampleCountFlagBits sampleCount);
		VkSampleCountFlagBits GetSampleCount() const;
	protected:
		void BindImage(const Vulkan::ImageView& imageView, const Vulkan::Sampler& sampler, uint32_t set, uint32_t binding, uint32_t array_element);
		void BindBuffer(const Vulkan::Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement);

		void FlushDescriptorSet(Vulkan::CommandBuffer& commandBuffer, Vulkan::PipelineLayout& pipelineLayout, uint32_t set);

		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders);

		VkSampleCountFlagBits sampleCount{ VK_SAMPLE_COUNT_1_BIT };

		Vulkan::RenderPass* renderPass = nullptr;
		Vulkan::Device& device;
	private:
		Vulkan::ShaderSource vertexShader;
		Vulkan::ShaderSource fragmentShader;

		std::vector<uint32_t> inputAttachments{};
		std::vector<uint32_t> outputAttachments{ 0 };
		std::vector<uint32_t> colorResolveAttachments{};

		std::map<uint32_t, BindingMap<VkDescriptorBufferInfo>> bufferBindings;
		std::map<uint32_t, BindingMap<VkDescriptorImageInfo>> imageBindings;

		bool disableDepthStencil = false;
	};
};