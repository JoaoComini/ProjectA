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

	struct HdrSettings
	{
		float exposure{ 1.0 };
	};

	struct ShadowSettings
	{
		float depthBias{ 0.05 };
		float normalBias{ 0.4 };
	};

	struct RendererSettings
	{
		HdrSettings hdr;
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
		std::pair<Camera&, glm::mat4&> GetMainCamera();

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
