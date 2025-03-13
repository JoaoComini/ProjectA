#include "JoltContactListener.h"

#include <Jolt/Physics/Collision/EstimateCollisionResponse.h> 

#include "Scene/Scene.h"

namespace Engine
{
    JoltContactListener::JoltContactListener(Scene& scene, JPH::PhysicsSystem& system) : scene(scene), system(system)
    {

    }

    void JoltContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& settings)
    {
        JPH::CollisionEstimationResult result;

        JPH::EstimateCollisionResponse(body1, body2, manifold, result, settings.mCombinedFriction, settings.mCombinedRestitution);

        auto entity1 = static_cast<Entity::Id>(body1.GetUserData());
        auto entity2 = static_cast<Entity::Id>(body2.GetUserData());

        if (! scene.Valid(entity1) || ! scene.Valid(entity2))
        {
            return;
        }

        scene.AddOrReplaceComponent<Component::PhysicsContactEnter>(entity1, entity2);
        scene.AddOrReplaceComponent<Component::PhysicsContactEnter>(entity2, entity1);
    }

    void JoltContactListener::OnContactRemoved(const JPH::SubShapeIDPair& pair)
    {
        JPH::CollisionEstimationResult result;

        auto& bodyInterface = system.GetBodyInterface();

        auto handle1 = bodyInterface.GetUserData(pair.GetBody1ID());
        auto handle2 = bodyInterface.GetUserData(pair.GetBody2ID());

        auto entity1 = static_cast<Entity::Id>(handle1);
        auto entity2 = static_cast<Entity::Id>(handle2);

        if (!scene.Valid(entity1) || !scene.Valid(entity2))
        {
            return;
        }

        scene.AddOrReplaceComponent<Component::PhysicsContactExit>(entity1, entity2);
        scene.AddOrReplaceComponent<Component::PhysicsContactExit>(entity2, entity1);
    }
}