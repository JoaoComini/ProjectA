#pragma once

#include "Widget.hpp"

#include "Scene/Scene.hpp"

#include "Vulkan/Device.hpp"
#include "Rendering/Texture.hpp"

#include <functional>

class Toolbar : public Widget
{
public:
	Toolbar(Engine::Scene& scene);
	~Toolbar();

	void Draw() override;

	void OnPlay(std::function<void()> onPlayFn);
	void OnStop(std::function<void()> onStopFn);

private:

	std::function<void()> onPlayFn;
	std::function<void()> onStopFn;

	Engine::Scene& scene;
};