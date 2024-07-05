#include "Renderer.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"

#include "Core/Window.hpp"

#include "Camera.hpp"
#include "Vertex.hpp"
#include "Resource/ResourceManager.hpp"
#include "RenderPipeline.hpp"
#include "RenderContext.hpp"

#include <array>

namespace Engine
{

	Renderer::Renderer(Window& window, Scene& scene)
	{
		context = std::make_unique<RenderContext>(window);
		pipeline = std::make_unique<RenderPipeline>(*context, scene);
	}

	Vulkan::CommandBuffer& Renderer::Begin()
	{
		return *(activeCommandBuffer = context->Begin());
	}

	void Renderer::Draw()
	{
		pipeline->Draw(*activeCommandBuffer);
	}

	void Renderer::End()
	{
		context->End(*activeCommandBuffer);
	}

	void Renderer::SetMainCamera(Camera& camera, glm::mat4 transform)
	{
		mainCamera = &camera;
		mainTransform = std::move(transform);
	}

	std::pair<Camera&, glm::mat4&> Renderer::GetMainCamera()
	{
		return { *mainCamera, mainTransform };
	}

	void Renderer::SetSettings(RendererSettings settings)
	{
		this->settings = settings;
	}

	RendererSettings Renderer::GetSettings() const
	{
		return settings;
	}

	RenderContext& Renderer::GetRenderContext()
	{
		return *context;
	}

	RenderPipeline& Renderer::GetRenderPipeline()
	{
		return *pipeline;
	}
}
