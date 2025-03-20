#include "JoltLayerMapper.h"

#include "JoltBroadPhaseLayer.h"

namespace Engine
{

	JoltLayerMapper::JoltLayerMapper()
	{
		mObjectToBroadPhase[Layer::NON_MOVING] = JoltBroadPhaseLayer::NON_MOVING;
		mObjectToBroadPhase[Layer::MOVING] = JoltBroadPhaseLayer::MOVING;
	}

	bool JoltLayerMapper::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
	{
		switch (inObject1)
		{
		case Layer::NON_MOVING:
			return inObject2 == Layer::MOVING;
		case Layer::MOVING:
			return true;
		default:
			return false;
		}
	}

	bool JoltLayerMapper::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
	{
		switch (inLayer1)
		{
		case Layer::NON_MOVING:
			return inLayer2 == JoltBroadPhaseLayer::MOVING;
		case Layer::MOVING:
			return true;
		default:
			JPH_ASSERT(false);
			return false;
		}
	}

	uint32_t JoltLayerMapper::GetNumBroadPhaseLayers() const
	{
		return JoltBroadPhaseLayer::NUM_LAYERS;
	}

	JPH::BroadPhaseLayer JoltLayerMapper::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const
	{
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	const char* JoltLayerMapper::GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)JoltBroadPhaseLayer::NON_MOVING:
			return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)JoltBroadPhaseLayer::MOVING:
			return "MOVING";
		default:
			return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

}