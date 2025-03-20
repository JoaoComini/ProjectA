#pragma once

#include "Widget.h"

class Toolbar : public Widget
{
public:
	void Draw(Engine::Scene& scene) override;

	void OnPlay(std::function<void()> onPlayFn);
	void OnStop(std::function<void()> onStopFn);

private:

	std::function<void()> onPlayFn;
	std::function<void()> onStopFn;
};