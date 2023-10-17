#pragma once

#include "../../Rendering/Vertex.hpp"

namespace flatbuffers {
    struct Vertex;

    Vertex Pack(const Engine::Vertex& obj);
    const Engine::Vertex UnPack(const Vertex& obj);
}