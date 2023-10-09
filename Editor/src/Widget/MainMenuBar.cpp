#include "MainMenuBar.hpp"

#include <imgui.h>



void MainMenuBar::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Import"))
			{
				if (onImportFn)
				{
					onImportFn();
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{
				if (onExitFn)
				{
					onExitFn();
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Metrics"))
			{
				openMetrics = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (openMetrics)
	{
		if (ImGui::Begin("Metrics", &openMetrics))
		{
			ImGuiIO& io = ImGui::GetIO();

			ImGui::Text("Frametime: %.3f ms/frame", 1000.0f / io.Framerate);
			ImGui::Text("Framerate: %.1f FPS", io.Framerate);
			ImGui::End();
		}
	}
}

void MainMenuBar::OnExit(std::function<void()> onExitFn)
{
	this->onExitFn = onExitFn;
}

void MainMenuBar::OnImport(std::function<void()> onImportFn)
{
	this->onImportFn = onImportFn;
}
