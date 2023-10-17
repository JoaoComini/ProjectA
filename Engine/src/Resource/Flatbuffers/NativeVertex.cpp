#include "Mesh_generated.h"

#include <flatbuffers/stl_emulation.h>

#include <glm/gtc/type_ptr.hpp>

namespace flatbuffers
{
    Vertex Pack(const Engine::Vertex& obj)
    {
        std::span<const float, 3> position{ glm::value_ptr(obj.position), 3 };
        std::span<const float, 3> normal{ glm::value_ptr(obj.normal), 3 };
        std::span<const float, 2> uv{ glm::value_ptr(obj.uv), 2 };

        return { position, normal, uv };
    }

    const Engine::Vertex UnPack(const Vertex& obj)
    {
        return Engine::Vertex{
            .position = glm::make_vec3(obj.position()->data()),
            .normal = glm::make_vec3(obj.normal()->data()),
            .uv = glm::make_vec2(obj.uv()->data()),
        };
    }
};