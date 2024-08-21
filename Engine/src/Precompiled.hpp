#pragma once

#include <type_traits>
#include <memory>
#include <algorithm>
#include <functional>

#include <atomic>
#include <thread>

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <queue>
#include <iterator>

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <stdexcept>
#include <random>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <cstring>

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include <glm/gtx/matrix_decompose.hpp>

#include <vulkan/vulkan.h>

#ifdef PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif

    #include <Windows.h>
#endif
