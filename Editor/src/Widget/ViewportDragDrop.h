#pragma once

#include "Widget.h"

#include <Resource/Resource.h>

using OnDropResourceFn = std::function<void(Engine::ResourceId, Engine::ResourceMapping)>;

class ViewportDragDrop final : public Widget
{
public:
    void Draw(Engine::SceneGraph& scene) override;
    void OnDropResource(OnDropResourceFn fn);

private:
    OnDropResourceFn onDropResourceFn;
};
