#include "Script.hpp"

namespace Engine
{
	Script::Script(std::string& code) : code(code)
	{

	}

	std::string_view Script::GetCode() const
	{
		return code;
	}

	ResourceType Script::GetStaticType()
	{
		return ResourceType::Script;
	}

	ResourceType Script::GetType() const
	{
		return GetStaticType();
	}
}