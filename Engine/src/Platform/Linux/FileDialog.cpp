#include "../FileDialog.hpp"

#ifdef PLATFORM_LINUX

namespace Engine::FileDialog
{
    std::string OpenFile(Window& window, const char* filter)
    {
		return std::string();
    }

	std::string SaveFile(Window& window, const char* filter)
	{
		return std::string();
	}

}

#endif // PLATFORM_LINUX
