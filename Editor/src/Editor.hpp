#pragma once

#include <Core/Application.hpp>
#include <Core/Main.hpp>

#include "System/CameraSystem.hpp"
#include "Widget/SceneHierarchy.hpp"
#include "Widget/EntityInspector.hpp"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor(ApplicationSpec& spec);

        void OnGui() override;
    private:
        void MainMenuBar();

        std::unique_ptr<SceneHierarchy> sceneHierarchy;
        std::unique_ptr<EntityInspector> entityInspector;
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

