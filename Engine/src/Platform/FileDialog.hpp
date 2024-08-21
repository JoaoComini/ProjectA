#pragma once

#include "Core/Window.hpp"

namespace Engine::FileDialog
{
	std::string OpenFile(Window& window, const char* filter);
	std::string SaveFile(Window& window, const char* filter);
};