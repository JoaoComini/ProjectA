#pragma once

#include <Core/Application.hpp>
#include <Core/Main.hpp>

#include "System/CameraSystem.hpp"
#include "Widget/SceneHierarchy.hpp"
#include "Widget/EntityInspector.hpp"
#include "Widget/MainMenuBar.hpp"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor(ApplicationSpec& spec);

        void OnGui() override;
    private:
        void ImportFile();

        std::unique_ptr<SceneHierarchy> sceneHierarchy;
        std::unique_ptr<EntityInspector> entityInspector;
        std::unique_ptr<MainMenuBar> mainMenuBar;

        bool openMetricsWindow = false;
    };

    std::unique_ptr<Application> CreateApplication()
    {
        ApplicationSpec spec
        {
            .name = "ProjectA"
        };

        return std::make_unique<Editor>(spec);
    }

}

