#include "JoltContactListener.hpp"

#include <Jolt/Physics/Collision/EstimateCollisionResponse.h> 

#include "Scene/Scene.hpp"

namespace Engine
{
    JoltContactListener::JoltContactListener(Scene& scene, JPH::PhysicsSystem& system) : scene(scene), system(system)
    {

    }

    void JoltContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& settings)
    {
        JPH::CollisionEstimationResult result;

        JPH::EstimateCollisionResponse(body1, body2, manifold, result, settings.mCombinedFriction, settings.mCombinedRestitution);

        auto entity1 = scene.FindEntityByHandle(static_cast<uint32_t>(body1.GetUserData()));
        auto entity2 = scene.FindEntityByHandle(static_cast<uint32_t>(body2.GetUserData()));

        if (! entity1 || ! entity2)
        {
            return;
        }

        entity1.AddOrReplaceComponent<Component::PhysicsContactEnter>(entity2);
        entity2.AddOrReplaceComponent<Component::PhysicsContactEnter>(entity1);
    }

    void JoltContactListener::OnContactRemoved(const JPH::SubShapeIDPair& pair)
    {
        JPH::CollisionEstimationResult result;

        auto& bodyInterface = system.GetBodyInterface();

        auto handle1 = bodyInterface.GetUserData(pair.GetBody1ID());
        auto handle2 = bodyInterface.GetUserData(pair.GetBody2ID());

        auto entity1 = scene.FindEntityByHandle(static_cast<uint32_t>(handle1));
        auto entity2 = scene.FindEntityByHandle(static_cast<uint32_t>(handle2));

        if (! entity1 || ! entity2)
        {
            return;
        }

        entity1.AddOrReplaceComponent<Component::PhysicsContactExit>(entity2);
        entity2.AddOrReplaceComponent<Component::PhysicsContactExit>(entity1);
    }
}