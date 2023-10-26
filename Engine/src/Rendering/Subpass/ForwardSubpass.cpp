#include "ForwardSubpass.hpp"


namespace Engine
{
    ForwardSubpass::ForwardSubpass(
        Vulkan::Device& device,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene
    ) : GeometrySubpass{device, std::move(vertexSource), std::move(fragmentSource), scene}
    {
    }

    void ForwardSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
    {
        LightUniform uniform{};

        auto [entity, found] = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

        if (found)
        {
            const auto& transform = entity.GetComponent<Component::Transform>();
            const auto& light = entity.GetComponent<Component::DirectionalLight>();

            uniform.color = { light.color, light.intensity };
            uniform.vector = glm::vec4{ transform.rotation * glm::vec3{ 0.f, 0.f, 1.f }, 1.f };
        }

        auto& frame = Renderer::Get().GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(LightUniform));

        allocation.SetData(&uniform);

        BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 3, 0);

        GeometrySubpass::Draw(commandBuffer);
    }
};