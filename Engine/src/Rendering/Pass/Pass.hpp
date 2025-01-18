#pragma once

#include "Rendering/RenderFrame.hpp"

namespace Vulkan
{
	class Sampler;
};

namespace Engine
{
	class RenderContext;

	class Pass
	{
	public:
		Pass(RenderContext& renderContext, ShaderSource&& vertexSource, ShaderSource&& fragmentSource);
		virtual ~Pass() = default;

		void Execute(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget);
		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) = 0;

		const ShaderSource& GetVertexShader() const;
		const ShaderSource& GetFragmentShader() const;

		RenderContext& GetRenderContext();

		virtual Vulkan::PipelineLayout& GetPipelineLayout(const std::vector<Shader*>& shaders);
	private:
		RenderContext& renderContext;

		ShaderSource vertexShader;
		ShaderSource fragmentShader;
	};
}