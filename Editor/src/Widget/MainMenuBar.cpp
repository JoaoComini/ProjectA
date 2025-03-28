#include "MainMenuBar.h"

#include <imgui.h>
#include <Scene/Scene.h>

void MainMenuBar::Draw(Engine::Scene& scene)
{
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		MainMenuItem("New Scene", onNewSceneFn);

		ImGui::Separator();

		MainMenuItem("Save Scene", onSaveSceneFn);

		ImGui::Separator();

		MainMenuItem("Import", onImportFn);

		ImGui::Separator();

		MainMenuItem("Exit", onExitFn);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		MainMenuItem("Metrics", [&]() {
			openMetrics = true;
		});

		MainMenuItem("Shadows", [&]() {
			openShadows = true;
		});

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	if (openMetrics)
	{
		ImGui::Begin("Metrics", &openMetrics);
		
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Frametime: %.3f ms/frame", 1000.0f / io.Framerate);
		ImGui::Text("Framerate: %.1f FPS", io.Framerate);
		ImGui::End();
		
	}

	if (openShadows)
	{
		ImGui::Begin("Shadows", &openShadows);

		//auto settings = Engine::Renderer::Get().GetShadowSettings();

		//ImGui::DragFloat("Depth Bias", &settings.depthBias, 0.01);
		//ImGui::DragFloat("Normal Bias", &settings.normalBias, 0.01);

		//Engine::Renderer::Get().SetShadowSettings(settings);

		ImGui::End();
	}
}

void MainMenuBar::OnExit(std::function<void()> onExitFn)
{
	this->onExitFn = onExitFn;
}

void MainMenuBar::OnSaveScene(std::function<void()> onSaveSceneFn)
{
	this->onSaveSceneFn = onSaveSceneFn;
}

void MainMenuBar::OnNewScene(std::function<void()> onNewSceneFn)
{
	this->onNewSceneFn = onNewSceneFn;
}

void MainMenuBar::OnImport(std::function<void()> onImportFn)
{
	this->onImportFn = onImportFn;
}

void MainMenuBar::MainMenuItem(std::string label, std::function<void()> callbackFn)
{
	if (ImGui::MenuItem(label.c_str()))
	{
		if (callbackFn)
		{
			callbackFn();
		}
	}
}
