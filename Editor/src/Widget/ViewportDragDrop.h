#pragma once

#include "Widget.h"

#include <Resource/Resource.h>

using OnDropResourceFn = std::function<void(Engine::ResourceId, Engine::ResourceMetadata)>;

class ViewportDragDrop : public Widget
{
public:
    
    virtual void Draw() override;

    void OnDropResource(OnDropResourceFn fn);

private:
    OnDropResourceFn onDropResourceFn;
};
