#pragma once

#include "Scene/Scene.h"
#include "Component/Group.h"

namespace Engine
{
    class SceneResource final : public Scene, public Resource
    {
    public:
        SceneResource() = default;

    	void Pack(const SceneGraph& graph);

    	[[nodiscard]] ResourceType GetType() const override;

        template<class Archive>
		void Save(Archive& ar) const
		{
        	entt::snapshot snapshot{ registry };

        	SnapshotEntities(ar, snapshot);
        	SnapshotComponentGroup(ar, snapshot, Component::Serializable);
		}

		template<class Archive>
		void Load(Archive& ar)
		{
        	entt::snapshot_loader snapshot{ registry };

        	SnapshotEntities(ar, snapshot);
        	SnapshotComponentGroup(ar, snapshot, Component::Serializable);

        	snapshot.orphans();
		}

    	template<class Archive, class Snapshot>
		static void SnapshotEntities(Archive& ar, Snapshot& snapshot)
        {
        	snapshot.template get<entt::entity>(ar);
        }

    	template<class Archive, class Snapshot, typename... Components>
		static void SnapshotComponentGroup(Archive& ar, Snapshot& snapshot, Component::Group<Components...>);
    };

	template<class Archive, class Snapshot, typename ... Components>
	void SceneResource::SnapshotComponentGroup(Archive &ar, Snapshot &snapshot, Component::Group<Components...>)
    {
    	([&]()
		{
			snapshot.template get<Components>(ar);
		}(), ...);
    }
}
