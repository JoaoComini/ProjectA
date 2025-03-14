#pragma once

namespace Engine
{
    class Scene;
    class RenderCamera;
    class RenderContext;
    class Primitive;
    class Material;

    struct RenderGeometry
    {
        glm::mat4 transform{};
		const Primitive* primitive{ nullptr };
		float distance{ 0.f };
    };

    class RenderBatcher
    {
    public:
        void BuildBatches(Scene& scene, const RenderCamera& camera);

		const std::vector<RenderGeometry>& GetOpaques();
		const std::vector<RenderGeometry>& GetTransparents();

		void Reset();

    private:
		Material& GetMaterial(const Primitive& primitive);
		void SortOpaques();
		void SortTransparents();

        std::vector<RenderGeometry> opaques;
        std::vector<RenderGeometry> transparents;
    };
}