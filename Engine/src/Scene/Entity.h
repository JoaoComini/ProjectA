#pragma once

#include <entt/entt.hpp>

namespace Engine
{
	namespace Entity
	{
		using Id = entt::entity;
		inline constexpr Id Null{ entt::null };
	};
};