#pragma once

#include "Common/Singleton.hpp"
#include "Resource/Resource.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "RenderPipeline.hpp"
#include "RenderContext.hpp"

namespace Engine
{
	class Camera;
	class Window;
	class Scene;

	struct ShadowSettings
	{
		float depthBias{ 4 };
		float normalBias{ 0.8 };
	};

	struct RendererSettings
	{
		ShadowSettings shadow;
	};

	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer(Window& window, Scene& scene);

		Vulkan::CommandBuffer& Begin();
		void Draw();
		void End();

		void SetMainCamera(Camera& camera, glm::mat4 transform);
		std::pair<Camera&, glm::mat4> GetMainCamera();

		void SetSettings(RendererSettings settings);
		RendererSettings GetSettings() const;

		RenderContext& GetRenderContext();
		RenderPipeline& GetRenderPipeline();
	private:
		std::unique_ptr<RenderContext> context;
		std::unique_ptr<RenderPipeline> pipeline;

		Vulkan::CommandBuffer* activeCommandBuffer{ nullptr };

		Camera mainCamera{};
		glm::mat4 mainTransform{};

		RendererSettings settings;
	};
}
