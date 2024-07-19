#pragma once

#include "Resource/Resource.hpp"

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

	private:
		std::string code;
	};
}