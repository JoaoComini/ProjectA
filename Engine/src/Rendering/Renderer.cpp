#include "Renderer.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"

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

	Uuid Renderer::AddMeshInstance(const MeshInstance& instance)
	{
		Uuid id{};

		meshInstances.emplace(id, instance);

		return id;
	}

	void Renderer::UpdateMeshInstance(Uuid id, const MeshInstance& instance)
	{
		meshInstances.insert_or_assign(id, instance);
	}

	const std::unordered_map<Uuid, MeshInstance>& Renderer::GetMeshInstances() const
	{
		return meshInstances;
	}

	void Renderer::RemoveMeshInstance(Uuid id)
	{
		meshInstances.erase(id);
	}

	Uuid Renderer::AddDirectionalLightInstance(const DirectionalLightInstance& instance)
	{
		Uuid id{};

		directionalLights.emplace(id, instance);

		return id;
	}

	void Renderer::UpdateDirectionalLightInstance(Uuid id, const DirectionalLightInstance& instance)
	{
		directionalLights.insert_or_assign(id, instance);
	}

	const std::unordered_map<Uuid, DirectionalLightInstance>& Renderer::GetDirectionalLights() const
	{
		return directionalLights;
	}

	void Renderer::RemoveDirectionalLightInstance(Uuid id)
	{
		directionalLights.erase(id);
	}

	Uuid Renderer::AddPointLightInstance(const PointLightInstance& instance)
	{
		Uuid id{};

		pointLights.emplace(id, instance);

		return id;
	}

	void Renderer::UpdatePointLightInstance(Uuid id, const PointLightInstance& instance)
	{
		pointLights.insert_or_assign(id, instance);
	}

	const std::unordered_map<Uuid, PointLightInstance>& Renderer::GetPointLights() const
	{
		return pointLights;
	}

	void Renderer::RemovePointLightInstance(Uuid id)
	{
		pointLights.erase(id);
	}

	void Renderer::SetSkyLight(SkyLight skyLight)
	{
		this->skyLight = skyLight;
	}

	SkyLight Renderer::GetSkyLight() const
	{
		return skyLight;
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
