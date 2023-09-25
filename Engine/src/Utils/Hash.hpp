#pragma once

#include <glm/gtx/hash.hpp>

#include "Rendering/Vertex.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"


inline void HashCombine(std::size_t& seed, std::size_t hash)
{
	hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= hash;
}

template<class T>
inline void HashCombine(std::size_t& seed, const T& t)
{
	std::hash<T> hasher;

	HashCombine(seed, hasher(t));
}

template<class T>
inline std::size_t Hash(const T& t)
{
	std::size_t result = 0U;

	HashCombine(result, t);

	return result;
}

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

	template <>
	struct hash<Vulkan::DescriptorSetLayout>
	{
		size_t operator()(const Vulkan::DescriptorSetLayout& layout) const
		{
			return Hash(layout.GetHandle());
		}
	};

}