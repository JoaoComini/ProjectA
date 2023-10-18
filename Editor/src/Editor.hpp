#pragma once

#include <Core/Application.hpp>
#include <Core/Main.hpp>

#include "Widget/SceneHierarchy.hpp"
#include "Widget/EntityInspector.hpp"
#include "Widget/MainMenuBar.hpp"
#include "Widget/ContentBrowser.hpp"
#include "Widget/ViewportDragDrop.hpp"

#include "EditorCamera.hpp"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor(ApplicationSpec& spec);

        void OnUpdate(float timestep) override;
        void OnGui() override;
        void OnWindowResize(int width, int height) override;

    private:
        void DrawViewportDragDrop(ImGuiID dockId);

        void NewScene();
        void SaveScene();
        void OpenScene(ResourceId id);

        void AddPrefabToScene(ResourceId id);
        void ImportFile();

        std::unique_ptr<EditorCamera> camera;

        std::unique_ptr<SceneHierarchy> sceneHierarchy;
        std::unique_ptr<EntityInspector> entityInspector;
        std::unique_ptr<MainMenuBar> mainMenuBar;
        std::unique_ptr<ContentBrowser> contentBrowser;
        std::unique_ptr<ViewportDragDrop> viewportDragDrop;
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

