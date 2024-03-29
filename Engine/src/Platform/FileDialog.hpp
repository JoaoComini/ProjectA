#pragma once

#include "Core/Window.hpp"

#include <string>

namespace Engine::FileDialog
{
	std::string OpenFile(Window& window, const char* filter);
	std::string SaveFile(Window& window, const char* filter);
};