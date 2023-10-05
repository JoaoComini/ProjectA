#pragma once

#include <glm/glm.hpp>

#include "Common/Hash.hpp"

namespace Engine
{

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        bool operator==(const Vertex& other) const
        {
            return position == other.position && normal == other.normal && uv == other.uv;
        }
    };
};

namespace std
{
	template <>
	struct hash<Engine::Vertex>
	{
		size_t operator()(const Engine::Vertex& vertex) const
		{
			std::size_t result = 0U;

			HashCombine(result, vertex.position);
			HashCombine(result, vertex.uv);
			HashCombine(result, vertex.normal);

			return result;
		}
	};
}
