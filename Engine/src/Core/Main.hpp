#pragma once

#include "Application.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    auto application = Engine::CreateApplication();

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