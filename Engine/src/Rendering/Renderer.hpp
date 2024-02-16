#pragma once


#include "Common/Singleton.hpp"
#include "Resource/Resource.hpp"
#include "Math/Transform.hpp"
#include "Vulkan/CommandBuffer.hpp"

#include "RenderPipeline.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Engine
{
	class Camera;
	class RenderContext;
	class Window;
	class Scene;

	struct HdrSettings
	{
		float exposure{ 1.0 };
	};

	struct RendererSettings
	{
		HdrSettings hdr;
	};

	struct MeshInstance
	{
		ResourceId mesh{ 0 };
		glm::mat4 transform{};
	};

	struct DirectionalLightInstance
	{
		Transform transform{};
		glm::vec3 color{1.f};
		float intensity = 1.f;
	};

	struct PointLightInstance
	{
		Transform transform{};
		glm::vec3 color{ 1.f };
		float range = 1.f;
	};

	struct SkyLight
	{
		ResourceId cubemap{ 0 };
	};

	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer(std::unique_ptr<RenderPipeline>&& renderPipeline);
		Renderer(Window& window, Scene& scene);

		Uuid AddMeshInstance(const MeshInstance& instance);
		void UpdateMeshInstance(Uuid id, const MeshInstance& instance);
		const std::unordered_map<Uuid, MeshInstance>& GetMeshInstances() const;
		void RemoveMeshInstance(Uuid id);

		Uuid AddDirectionalLightInstance(const DirectionalLightInstance& instance);
		void UpdateDirectionalLightInstance(Uuid id, const DirectionalLightInstance& instance);
		const std::unordered_map<Uuid, DirectionalLightInstance>& GetDirectionalLights() const;
		void RemoveDirectionalLightInstance(Uuid id);

		Uuid AddPointLightInstance(const PointLightInstance& instance);
		void UpdatePointLightInstance(Uuid id, const PointLightInstance& instance);
		const std::unordered_map<Uuid, PointLightInstance>& GetPointLights() const;
		void RemovePointLightInstance(Uuid id);

		void SetSkyLight(SkyLight skyLight);
		SkyLight GetSkyLight() const;

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

		Camera* mainCamera{nullptr};
		glm::mat4 mainTransform{};

		RendererSettings settings;

		std::unordered_map<Uuid, MeshInstance> meshInstances;
		std::unordered_map<Uuid, DirectionalLightInstance> directionalLights;
		std::unordered_map<Uuid, PointLightInstance> pointLights;

		SkyLight skyLight;
	};
}
