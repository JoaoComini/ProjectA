#pragma once

namespace glm
{
    template <class Archive>
    void Serialize(Archive& ar, vec2& vec)
    {
        ar(vec.x, vec.y);
    }

    template <class Archive>
    void Serialize(Archive& ar, vec3& vec)
    {
        ar(vec.x, vec.y, vec.z);
    }
}

namespace Engine
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;

        bool operator==(const Vertex& other) const
        {
            return position == other.position && normal == other.normal && uv == other.uv;
        }
    };

    template <class Archive>
    void Serialize(Archive& ar, Vertex& vertex)
    {
        ar(vertex.position, vertex.normal, vertex.uv);
    }
};