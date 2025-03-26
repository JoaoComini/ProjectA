#pragma once

#include "Scene.h"

namespace Engine
{
    class SceneResource final : public Scene, public Resource
    {
    public:
        SceneResource() = default;

    	[[nodiscard]] ResourceType GetType() const override;

        template<class Archive>
		void Save(Archive& ar) const
		{
			SaveSnapshot(ar, *this);
		}

		template<class Archive>
		void Load(Archive& ar)
		{
			LoadSnapshot(ar, *this);
		}
    };

}
