#pragma once

namespace Engine
{
    class Scene;
    class RenderCamera;
    class Primitive;
    class Material;

    struct RenderGeometry
    {
        glm::mat4 transform;
		Primitive* primitive{ nullptr };
		Material* material{ nullptr };
		float distance{ 0.f };
    };

    class RenderBatcher
    {
    public:
        void BuildBatches(Scene& scene, RenderCamera& camera);

		const std::vector<RenderGeometry>& GetOpaques();
		const std::vector<RenderGeometry>& GetTransparents();

		void Reset();

    private:

		Material& GetMaterial(Primitive& primitive);
		void SortOpaques();
		void SortTransparents();


        std::vector<RenderGeometry> opaques;
        std::vector<RenderGeometry> transparents;
    };
}