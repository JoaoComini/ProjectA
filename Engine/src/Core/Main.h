#pragma once

#include "Application.h"

int main(int argc, char** argv)
{
    auto application = Engine::CreateApplication({ argc, {argv, argv + argc}});

    try
    {
        application->Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}