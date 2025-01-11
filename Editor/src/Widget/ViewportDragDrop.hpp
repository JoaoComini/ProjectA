#pragma once

#include "Widget.hpp"

#include <Resource/Resource.hpp>

using OnDropResourceFn = std::function<void(Engine::ResourceId, Engine::ResourceMetadata)>;

class ViewportDragDrop : public Widget
{
public:
    
    virtual void Draw() override;

    void OnDropResource(OnDropResourceFn fn);

private:
    OnDropResourceFn onDropResourceFn;
};
