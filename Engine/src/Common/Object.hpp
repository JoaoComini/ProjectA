#pragma once

#include <string_view>

#define REFLECT(class)                                      \
                                                            \
public:                                                     \
    constexpr static std::string_view GetStaticClass()      \
    {                                                       \
        return #class;                                      \
    }                                                       \
                                                            \
    virtual constexpr std::string_view GetClass() override  \
    {                                                       \
        return #class;                                      \
    }                                                       \


namespace Engine
{
    class Object
    {
    public:
        constexpr static std::string_view GetStaticClass()
        {
            return "Object";
        }

        virtual constexpr std::string_view GetClass()
        {
            return "Object";
        }
    };
}