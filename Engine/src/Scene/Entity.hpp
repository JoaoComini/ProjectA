#pragma once

#include <entt/entt.hpp>

#include "Common/Uuid.hpp"

namespace Engine
{
	namespace Entity
	{
		using Id = entt::entity;
		inline constexpr Entity::Id Null{ entt::null };
	};
};