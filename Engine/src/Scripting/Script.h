#pragma once

#include "Resource/Resource.h"

#include <cereal/types/string.hpp>

namespace Engine
{
	class Script : public Resource
	{
	public:
		Script() = default;
		Script(std::string& code);

		std::string_view GetCode() const;

		static ResourceType GetStaticType();
		ResourceType GetType() const override;

		static std::string GetExtension()
		{
			return "lua";
		}

		template<typename Archive>
		void Serialize(Archive& ar)
		{
			ar(code);
		}

	private:
		std::string code;
	};
}