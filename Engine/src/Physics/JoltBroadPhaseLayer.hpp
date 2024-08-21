#pragma once

namespace Engine
{
	namespace JoltBroadPhaseLayer
	{
		static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
		static constexpr JPH::BroadPhaseLayer MOVING(1);
		static constexpr uint32_t NUM_LAYERS(2);
	};

}
