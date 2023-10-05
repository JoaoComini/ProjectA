#pragma once

#include "Widget.hpp"

#include <functional>

class MainMenuBar : public Widget
{
public:
	void Update() override;

	void OnExit(std::function<void()> onExitFn);
	void OnImport(std::function<void()> onImportFn);

private:
	std::function<void()> onExitFn;
	std::function<void()> onImportFn;

	bool openMetrics = false;
};

