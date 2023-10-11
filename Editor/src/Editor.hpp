#pragma once

#include <Core/Application.hpp>
#include <Core/Main.hpp>

#include "System/CameraSystem.hpp"
#include "Widget/SceneHierarchy.hpp"
#include "Widget/EntityInspector.hpp"
#include "Widget/MainMenuBar.hpp"
#include "Widget/ContentBrowser.hpp"
#include "Scene/Mixin.hpp"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor(ApplicationSpec& spec);

        void OnGui() override;
    private:
        void SaveScene();
        void OpenScene();
        void ImportFile();

        std::filesystem::path scenePath;

        std::unique_ptr<SceneHierarchy> sceneHierarchy;
        std::unique_ptr<EntityInspector> entityInspector;
        std::unique_ptr<MainMenuBar> mainMenuBar;
        std::unique_ptr<ContentBrowser> contentBrowser;

        bool openMetricsWindow = false;
    };

    std::unique_ptr<Application> CreateApplication(ApplicationArgs args)
    {
        ApplicationSpec spec
        {
            .name = "ProjectA",
            .args = args
        };

        return std::make_unique<Editor>(spec);
    }

}

