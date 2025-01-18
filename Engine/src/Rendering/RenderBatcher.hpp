#pragma once


namespace Engine
{
    class Scene;
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
        RenderBatcher(Scene& scene);

        void BuildBatches();

		const std::vector<RenderGeometry>& GetOpaques();
		const std::vector<RenderGeometry>& GetTransparents();

		void Reset();

    private:

		Material& GetMaterial(Primitive& primitive);
		void SortOpaques();
		void SortTransparents();


        std::vector<RenderGeometry> opaques;
        std::vector<RenderGeometry> transparents;

        Scene& scene;
    };
}