#include "Renderer.hpp"

#include "Vulkan/CommandBuffer.hpp"

#include "Core/Window.hpp"

#include "Resource/ResourceManager.hpp"
#include "RenderContext.hpp"

#include "RenderGraph/RenderGraph.hpp"

#include "VulkanRenderGraphAllocator.hpp"
#include "VulkanRenderGraphCommand.hpp"

namespace Engine
{
	Renderer::Renderer(RenderContext& renderContext)
		: renderContext(renderContext)
	{
		allocator = std::make_unique<VulkanRenderGraphAllocator>(renderContext.GetDevice());

		VkSamplerCreateInfo point{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		point.magFilter = VK_FILTER_NEAREST;
		point.minFilter = VK_FILTER_NEAREST;
		point.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		point.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.mipLodBias = 0.0f;
		point.minLod = 0.0f;
		point.maxLod = 1.0f;
		point.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		VkSamplerCreateInfo shadow{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		shadow.minFilter = VK_FILTER_LINEAR;
		shadow.magFilter = VK_FILTER_LINEAR;
		shadow.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		shadow.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		shadow.compareEnable = VK_TRUE;
		shadow.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

		samplers.emplace(RenderTextureSampler::Point, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), point));
		samplers.emplace(RenderTextureSampler::Shadow, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), shadow));
	}

	Renderer::~Renderer()
	{
		allocator.reset();
		samplers.clear();
	}

	void Renderer::Draw(Vulkan::CommandBuffer& commandBuffer, Scene& scene, RenderCamera& camera, RenderAttachment& target)
	{
		RenderBatcher batcher;
		batcher.BuildBatches(scene, camera);

		const auto [width, height] = target.GetExtent();

		RenderGraph graph;
		auto& graphContext = graph.GetContext();

		ImportFrameData(graph, graphContext, camera);
		ImportBackBufferData(graph, graphContext, target);
		ImportLightsData(graph, graphContext, scene);

		ShadowPass shadowPass{ scene, settings.shadow };
		ForwardPass forwardPass{ scene, { width, height } };
		CompositionPass compositionPass;

		graph.AddPass(shadowPass);
		graph.AddPass(forwardPass);
		graph.AddPass(compositionPass);

		graph.Compile();

		VulkanRenderGraphCommand command{ renderContext, batcher, shaderCache, commandBuffer, samplers };
		graph.Execute(command, *allocator);
	}

	struct CameraUniform
	{
		glm::mat4 viewProjectionMatrix;
		glm::vec3 position;
	};

	void Renderer::ImportFrameData(RenderGraph& graph, RenderGraphContext& context, RenderCamera &camera) const
	{
		const auto transform = camera.GetTransform();
		const auto projection = camera.GetProjection();

		CameraUniform cameraUniform{
			.viewProjectionMatrix = projection * glm::inverse(transform),
			.position = camera.GetPosition(),
		};

		auto allocation = renderContext
			.GetCurrentFrame()
			.RequestBufferAllocation(Vulkan::BufferUsageFlags::Uniform, sizeof(CameraUniform));

		allocation.SetData(&cameraUniform);

		auto& frameData = context.Add<FrameData>();
		frameData.camera = graph.Import<RenderBuffer>(
			{ allocation },
			{ sizeof(CameraUniform) }
		);
	}

	void Renderer::ImportBackBufferData(RenderGraph& graph, RenderGraphContext& context, RenderAttachment &target) const
	{
		auto& backBufferData = context.Add<BackBufferData>();
		backBufferData.target = graph.Import<RenderTexture>(
			{ &target },
			{
				.width = target.GetExtent().width,
				.height = target.GetExtent().height,
				.format = RenderTextureFormat::Linear,
				.usage = RenderTextureUsage::RenderTarget
			}
		);
	}

	struct Light
	{
		glm::vec4 vector;
		glm::vec4 color;
	};

	struct alignas(16) LightsUniform
	{
		Light lights[32];
		int count;
	};

	struct ShadowUniform
	{
		glm::mat4 viewProjection;
	};

	void GetMainLightData(Scene& scene, LightsUniform& lights, ShadowUniform& shadow)
	{
		const auto query = scene.Query<Component::Transform, Component::DirectionalLight>();

		const auto entity = query.First();

		if (entity == Entity::Null)
		{
			return;
		}

		const auto& [transform, light] = query.GetComponent(entity);

		auto direction = glm::normalize(transform.rotation * glm::vec3{ 0, 0, 1 });
		auto view = glm::lookAt(-direction, glm::vec3{ 0, 0, 0 }, glm::vec3{ 0, 1, 0 });

		Camera camera;
		camera.SetOrthographic(50, -25.f, 25.f);

		auto projection = camera.GetProjection();

		shadow.viewProjection = projection * view;

		lights.lights[0].color = { light.color, light.intensity };
		lights.lights[0].vector = glm::vec4{ direction, 1.f };

		lights.count++;
	}

	void GetAdditionalLightsData(Scene& scene, LightsUniform& uniform)
	{
		auto query = scene.Query<Component::Transform, Component::PointLight>();

		for (auto entity : query)
		{
			auto count = uniform.count;

			const auto& [transform, light] = query.GetComponent(entity);

			uniform.lights[count].color = { light.color, light.range };
			uniform.lights[count].vector = { transform.position, 0.f };

			uniform.count++;
		}
	}

	void Renderer::ImportLightsData(RenderGraph &graph, RenderGraphContext &context, Scene &scene) const
	{
		LightsUniform lights{};
		ShadowUniform shadow{};

		GetMainLightData(scene, lights, shadow);
		GetAdditionalLightsData(scene, lights);

		auto& lightData = context.Add<LightData>();

		{
			auto allocation = renderContext
			.GetCurrentFrame()
			.RequestBufferAllocation(Vulkan::BufferUsageFlags::Uniform, sizeof(LightsUniform));

			allocation.SetData(&lights);

			lightData.lights = graph.Import<RenderBuffer>(
				{ allocation },
				{ sizeof(CameraUniform) }
			);
		}

		{
			auto allocation = renderContext
			.GetCurrentFrame()
			.RequestBufferAllocation(Vulkan::BufferUsageFlags::Uniform, sizeof(ShadowUniform));

			allocation.SetData(&shadow);

			lightData.shadows = graph.Import<RenderBuffer>(
				{ allocation },
				{ sizeof(CameraUniform) }
			);
		}
	}


}
