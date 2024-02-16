#pragma once

#include <imgui.h>

#include <Scene/Node/Node.hpp>

#include "Widget.hpp"
#include "Controls.hpp"

class NodeInspector : public Widget
{
public:

    void SetNode(Engine::Node* node);
    void Draw() override;

private:
    Engine::Node* node;
};