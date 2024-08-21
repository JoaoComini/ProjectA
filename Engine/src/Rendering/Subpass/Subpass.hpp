#pragma once

#include "Rendering/Camera.hpp"
#include "Rendering/RenderContext.hpp"
#include "Rendering/RenderFrame.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"
#include "Vulkan/Sampler.hpp"

namespace Engine
{
	class Subpass
	{
	public:
		Subpass(RenderContext& renderContext, Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource);
		virtual ~Subpass() = default;

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) = 0;

		const Vulkan::ShaderSource& GetVertexShader() const;
		const Vulkan::ShaderSource& GetFragmentShader() const;
	protected:
		RenderContext& GetRenderContext();

		void BindImage(const Vulkan::ImageView& imageView, const Vulkan::Sampler& sampler, uint32_t set, uint32_t binding, uint32_t array_element);
		void BindBuffer(const Vulkan::Buffer& buffer, uint32_t offset, uint32_t size, uint32_t set, uint32_t binding, uint32_t arrayElement);

		void FlushDescriptorSet(Vulkan::CommandBuffer& commandBuffer, Vulkan::PipelineLayout& pipelineLayout, uint32_t set);

		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders);
	private:
		RenderContext& renderContext;

		Vulkan::ShaderSource vertexShader;
		Vulkan::ShaderSource fragmentShader;

		std::map<uint32_t, BindingMap<VkDescriptorBufferInfo>> bufferBindings;
		std::map<uint32_t, BindingMap<VkDescriptorImageInfo>> imageBindings;

		bool disableDepthStencil = false;
	};
};