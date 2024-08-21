#pragma once

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
inline void Hash(std::size_t& seed, const T& t)
{
	HashCombine(seed, t);
}

template <typename T, typename... Args>
inline void Hash(std::size_t& seed, const T& first, const Args &... args)
{
	Hash(seed, first);
	Hash(seed, args...);
}

namespace std
{
	template<typename T>
	struct hash<std::vector<T>>
	{
		size_t operator()(const std::vector<T>& vec) const
		{
			std::size_t hash{ 0 };

			for (const auto& item : vec)
			{
				HashCombine(hash, item);
			}

			return hash;
		}
	};
}