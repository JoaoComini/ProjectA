#pragma once

#include <Core/Application.h>
#include <Core/Main.h>

#include "Widget/SceneHierarchy.h"
#include "Widget/EntityInspector.h"
#include "Widget/MainMenuBar.h"
#include "Widget/ContentBrowser.h"
#include "Widget/ViewportDragDrop.h"
#include "Widget/EntityGizmo.h"
#include "Widget/Toolbar.h"

#include "EditorCamera.h"
#include "Util/FileWatcher.h"

namespace Engine
{
    class Editor final : public Application
    {
    public:
        explicit Editor(ApplicationSpec& spec);
        ~Editor() override;

        void OnUpdate(float timestep) override;
        void OnGui() override;
        void OnWindowResize(int width, int height) override;
        void OnInputEvent(const InputEvent& event) override;
    private:
        void NewScene();
        void SaveScene();
        void OpenScene(ResourceId id);

        void AddScene(ResourceId id);
        void AddSkyLightToScene(ResourceId id);

        void ImportFile();

        std::unique_ptr<EditorCamera> camera;

        std::unique_ptr<SceneHierarchy> sceneHierarchy;
        std::unique_ptr<EntityInspector> entityInspector;
        std::unique_ptr<MainMenuBar> mainMenuBar;
        std::unique_ptr<ContentBrowser> contentBrowser;
        std::unique_ptr<ViewportDragDrop> viewportDragDrop;
        std::unique_ptr<EntityGizmo> entityGizmo;
        std::unique_ptr<Toolbar> toolbar;

        std::unique_ptr<FileWatcher> fileWatcher;

        ResourceId currentScene{ 0 };
    };

    inline std::unique_ptr<Application> CreateApplication(const ApplicationArgs &args)
    {
        ApplicationSpec spec
        {
            .name = "ProjectA",
            .args = args
        };

        return std::make_unique<Editor>(spec);
    }

}

