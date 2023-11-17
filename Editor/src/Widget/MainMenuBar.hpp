#pragma once

#include "Widget.hpp"

#include <functional>
#include <string>

class MainMenuBar : public Widget
{
public:
	void Draw() override;

	void OnExit(std::function<void()> onExitFn);
	void OnSaveScene(std::function<void()> onSaveSceneFn);
	void OnNewScene(std::function<void()> onNewSceneFn);
	void OnImport(std::function<void()> onImportFn);

private:
	std::function<void()> onExitFn;
	std::function<void()> onImportFn;
	std::function<void()> onSaveSceneFn;
	std::function<void()> onNewSceneFn;

	void MainMenuItem(std::string label, std::function<void()> callbackFn);

	bool openMetrics = false;
	bool openEnvironment = false;
};

